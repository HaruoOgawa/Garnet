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

	bool CVulkanAPI::Initialize()
	{
		if (!CreateInstance()) return false; // インスタンスを作成
		if (!SetupDebugMessengerEXT()) return false; // インスタンス生成時に設定したプリセットのDebugMessengerだけではカバーできない範囲のハンドリング

		return true;
	}

	void CVulkanAPI::Release()
	{
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

	// 初期化関連の関数
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

	// ヘルパー関数 ////////////////////////////////////////////////////
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
		Console::Log(pCallbackData->pMessage);

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
}