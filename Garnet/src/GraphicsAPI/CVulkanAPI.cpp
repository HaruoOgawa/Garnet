#include "CVulkanAPI.h"
#include "CVulkanRenderer.h"
#include "../Debug/Message/Console.h"

namespace api
{
	CVulkanAPI::CVulkanAPI()
	{
	}

	CVulkanAPI::~CVulkanAPI()
	{
		
	}

	bool CVulkanAPI::Initialize(GLFWwindow* pWindow)
	{
		if (!CreateInstance()) return false; // インスタンスを作成
		if (!SetupDebugMessengerEXT()) return false; // インスタンス生成時に設定したプリセットのDebugMessengerだけではカバーできない範囲のハンドリング
		if (!CreateSurface(pWindow)) return false; // ウィンドウサーフェイスを作成(ウィンドウシステムとやり取りをする箇所)
		if (!CreateDevices()) return false; // デバイスを作成(物理デバイス/論理デバイス)

		return true;
	}

	void CVulkanAPI::Release()
	{
		// デバイスの破棄
		vkDestroyDevice(m_LogicalDevice, nullptr);

		// ウィンドウサーフェイスの破棄
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

		// 検証レイヤーを使用しているのなら破棄する
		if (m_IsUseDebugValidationLayer) {
			DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
		}

		// インスタンスを破棄
		vkDestroyInstance(m_Instance, nullptr);
	}

	std::shared_ptr<renderer::IRenderer> CVulkanAPI::CreateRenderer()
	{
		const auto Renderer = std::make_shared<renderer::CVulkanRenderer>();

		return Renderer;
	}

	// 初期化関連の関数 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// インスタンスを作成
	bool CVulkanAPI::CreateInstance()
	{
		VkResult result = VK_SUCCESS;

		// 使用可能レイヤーリストの初期化
		InitAvailableLayerList();

		// デバッグ用の検証レイヤーが使用可能かチェックする
		if (m_IsUseDebugValidationLayer && !CheckDebugValidationLayerSupport()) return false;

		// アプリケーション情報
		VkApplicationInfo AppInfo{};
		AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		AppInfo.pApplicationName = "Garnet";
		AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		AppInfo.pEngineName = "Garnet";
		AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		AppInfo.apiVersion = VK_API_VERSION_1_0;

		// インスタンス情報 
		VkInstanceCreateInfo InstanceInfo{};
		InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		InstanceInfo.pApplicationInfo = &AppInfo;

		// デバッグメッセージを出す用のレイヤー
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (m_IsUseDebugValidationLayer)
		{
			// レイヤー(Layer) => インスタンスレイヤー => APIをラップして使う感じの拡張機能
			// エクステンション(Extension) => 単純にAPIから派生させて生やす感じの拡張機能

			//
			InstanceInfo.enabledLayerCount = static_cast<uint32_t>(m_UseLayerList.size()); // 使用する拡張機能レイヤーの数
			InstanceInfo.ppEnabledLayerNames = &m_UseLayerList[0]; // 使用する拡張機能レイヤーの名前リスト
		
			SetDebugMessengerCreateInfo(debugCreateInfo); // 使用する拡張機能本体の設定
			InstanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo; // pNextは拡張機能レイヤーの生成情報  
		}
		else
		{
			InstanceInfo.enabledLayerCount = 0;
			InstanceInfo.ppEnabledLayerNames = nullptr;
		}

		// GLFWの拡張機能を設定(たぶんここで外部ツールの拡張機能を設定できる？)
		auto extensions = GetRequiredExtensions();
		InstanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		InstanceInfo.ppEnabledExtensionNames = &extensions[0];

		// インスタンスを作成
		result = vkCreateInstance(&InstanceInfo, nullptr, &m_Instance);

		return (result == VK_SUCCESS);
	}

	// Vulkanのウィンドウサーフェイスを作成(ウィンドウシステムとやり取りをする箇所)
	bool CVulkanAPI::CreateSurface(GLFWwindow* pWindow)
	{
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = glfwGetWin32Window(pWindow); // ウィンドウへのハンドル
		createInfo.hinstance = GetModuleHandle(nullptr); // 現在のプロセスへのハンドル

		if (vkCreateWin32SurfaceKHR(m_Instance, &createInfo, nullptr, &m_Surface) != VK_SUCCESS)
		{
			Console::Log("[Error] vkCreateWin32SurfaceKHR\n");
			return false;
		}

		// VulkanのウィンドウサーフェイスとGLFWを結び付ける
		if (glfwCreateWindowSurface(m_Instance, pWindow, nullptr, &m_Surface) != VK_SUCCESS)
		{
			Console::Log("[Error] glfwCreateWindowSurface\n");
			return false;
		}

		return true;
	}

	bool CVulkanAPI::CreateDevices()
	{
		// 物理デバイスの取得
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);

		// 物理デバイスのハンドル用配列を取得する
		std::vector<VkPhysicalDevice> PhysicalDeviceList;
		PhysicalDeviceList.resize(physicalDeviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, &PhysicalDeviceList[0]);

		// 使用したい機能を持っている物理デバイスを探す
		bool IsFindDevice = false;
		for (const auto& device : PhysicalDeviceList)
		{
			if (IsDeviceSuitable(device))
			{
				m_PhysicalDevice = device;
				IsFindDevice = true;

				break;
			}
		}

		if (!IsFindDevice)
		{
			Console::Log("[Error] Can not find device.\n");
			return false;
		}

		// 論理デバイスの作成 ///////////////////////////////////////////////////////////////
		// 物理デバイスのオプション機能を問い合わせる
		// (ベンダー名やグラボ名などの基本的な情報はvkGetPhysicalDevicePropertiesで問い合わせる)
		// vkGetPhysicalDeviceFeaturesでジオメトリシェーダー等の機能を処理対象グラボが持っているか確認する

		// デフォルトで有効になっている設定一覧を取得する 
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &supportedFeatures);

		// <<C++構文>> "{ }" のコンストラクターについて
		// コンストラクタが単純な構造体は初期化時に構造体名のコンストラクタを呼ばずに簡潔に中括弧{}のみで初期化することができる.
		// https://learn.microsoft.com/ja-jp/cpp/cpp/initializing-classes-and-structs-without-constructors-cpp?view=msvc-170
		// https://www.ibm.com/docs/ja/zos/2.3.0?topic=initializers-initialization-structures-unions

		// 必要なオプション機能を有効にする
		VkPhysicalDeviceFeatures requiredFeatures = {};
		requiredFeatures.multiDrawIndirect = supportedFeatures.multiDrawIndirect;
		requiredFeatures.tessellationShader = VK_TRUE;
		requiredFeatures.geometryShader = VK_TRUE;

		// ファミリーキューの設定
		QueueFamiryIndices indices = FindQueueFamilies(m_PhysicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.m_GraphicsFamily.value(), indices.m_PresentFamily.value() };
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		// 論理デバイスの設定
		VkDeviceCreateInfo deviceCreateInfo = {  };
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		// 基本的な設定
		deviceCreateInfo.pEnabledFeatures = &requiredFeatures;

		// 論理デバイスのQueue
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfos[0];

		// 論理デバイスの拡張機能の設定
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = &m_DeviceExtensions[0];

		// レイヤーの設定
		if (m_IsUseDebugValidationLayer)
		{
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_UseLayerList.size());
			deviceCreateInfo.ppEnabledLayerNames = &m_UseLayerList[0];
		}
		else
		{
			deviceCreateInfo.enabledLayerCount = 0;
		}

		// 論理デバイスを作成
		VkResult result = vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_LogicalDevice);

		// キューへのハンドルを取得
		vkGetDeviceQueue(m_LogicalDevice, indices.m_GraphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_LogicalDevice, indices.m_PresentFamily.value(), 0, &m_PresentQueue);

		return (result == VK_SUCCESS);
	}

	// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// レイヤー
	void CVulkanAPI::InitAvailableLayerList()
	{
		// レイヤーとは拡張機能のこと
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		m_AvailableLayers.resize(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, &m_AvailableLayers[0]);
	}
	
	bool CVulkanAPI::CheckDebugValidationLayerSupport()
	{
		const auto& LayerName = m_UseLayerList[0];

		// 名前を比較して拡張機能をサポートしているかチェックする
		for (const auto& layerProperties : m_AvailableLayers)
		{
			if (std::strcmp(LayerName, layerProperties.layerName) == 0)
			{
				return true;
			}
		}

		return false;
	}

	void CVulkanAPI::SetDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		// 捕捉するメッセージの重要性の種類(冗長, 警告, エラー)
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT 
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		// コールバックが通知されるメッセージの種類(一般的, 検証用, 性能)
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		// 呼び出すコールバック 
		createInfo.pfnUserCallback = DebugCallback;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL CVulkanAPI::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageServerity,
		VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		std::string message(pCallbackData->pMessage);

		if(messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) message = "[GENERAL]" + message + "\n";
		else if(messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) message = "[VALIDATION]" + message + "\n";
		else if(messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) message = "[PERFORMANCE]" + message + "\n";
		
		Console::Log(message.c_str());

		return VK_FALSE;
	}

	std::vector<const char*> CVulkanAPI::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (m_IsUseDebugValidationLayer)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	bool CVulkanAPI::SetupDebugMessengerEXT()
	{
		if (!m_IsUseDebugValidationLayer) return true;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		SetDebugMessengerCreateInfo(createInfo);

		VkResult result = CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger);
		return (result == VK_SUCCESS);
	}

	VkResult CVulkanAPI::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
		const VkAllocationCallbacks * pAllocator, VkDebugUtilsMessengerEXT * pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void CVulkanAPI::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks * pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}


	// デバイス
	bool CVulkanAPI::IsDeviceSuitable(VkPhysicalDevice device)
	{
		// 物理デバイスが持つキューファミリーを探す
		// キューとは単純に待機中のコマンドの列。しかしキューにはグラフィックコマンドを処理するものだったりメモリだったり役割が異なるいくつかの種類がある
		// キューファミリはそのキューの役割を説明するもの。(たぶんクラス的にはこれがキューのプロパティも持っている)
		QueueFamiryIndices indices = FindQueueFamilies(device);

		// 使用したい物理デバイス拡張機能が有効かチェックする
		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		// デバイスが持っているスワップチェーン(画面に表示されるのを待っている画像のキューのこと)のキューの拡張機能をチェックする
		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.m_Formats.empty() && !swapChainSupport.m_PresentModes.empty();
		}

		// 物理デバイスが持っている機能をチェックする(例えば64bit float textureが使えるか, テクスチャ圧縮, マルチビューポート)
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	bool CVulkanAPI::CheckDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, &availableExtensions[0]);

		std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	// Queue
	QueueFamiryIndices CVulkanAPI::FindQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamiryIndices indices;

		// 使用可能なキューファミリのリストを取得する
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, &queueFamilies[0]);

		// リストの確認
		for (int i = 0; i < queueFamilies.size(); i++)
		{
			const auto& QueueFamily = queueFamilies[i];

			// グラフィックキューファミリが使用できるか
			// &演算: 両方1なら1, そうでないなら0. if文は0でなければtrueを返す
			if (QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.m_GraphicsFamily = i;
			}

			//プレゼントキューファミリが使用できるかをウィンドウサーフェイスに問い合わせる
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
			if (presentSupport)
			{
				indices.m_PresentFamily = i;
			}
		}

		return indices;
	}

	// Presentation
	// SwapChainの使用可否の詳細を問い合わせる
	// ウィンドウサーフェイスがSwapChainに対応しているか確認する 
	SwapChainSupportDetails CVulkanAPI::QuerySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;

		// スワップ チェーン内の画像の最小/最大数、画像の最小/最大幅と高さ
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.m_Capabilities);

		// 表面形式 (ピクセル形式、色空間)
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.m_Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, &details.m_Formats[0]);
		}

		// 利用可能なプレゼンテーション モード
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.m_PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, &details.m_PresentModes[0]);
		}

		return details;
	}
}