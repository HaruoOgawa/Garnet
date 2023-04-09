#ifndef __DAWN__
#include "CVulkanAPI.h"
#include "CVulkanRenderer.h"
#include "../Debug/Message/Console.h"

namespace api
{
	CVulkanAPI::CVulkanAPI():
		m_pWindow(nullptr),
		m_CurrentImageIndex(0),
		m_IsReCreateSwapChain(false),
		m_WaitRendering(true)
	{
	}

	CVulkanAPI::~CVulkanAPI()
	{
		
	}

	bool CVulkanAPI::InitializeWithGLFW(GLFWwindow* pWindow)
	{
		m_pWindow = pWindow;

		if (!CreateInstance()) return false; // インスタンスを作成
		if (!SetupDebugMessengerEXT()) return false; // インスタンス生成時に設定したプリセットのDebugMessengerだけではカバーできない範囲のハンドリング
		if (!CreateSurface()) return false; // ウィンドウサーフェイスを作成(ウィンドウシステムとやり取りをする箇所)
		if (!CreateDevices()) return false; // デバイスを作成(物理デバイス/論理デバイス)
		if (!CreateSwapChain()) return false; // スワップチェインを作成(画面に示されるのを待っている画像のキューのマネージャーこと)
		if (!CreateImageViews()) return false; // イメージビューの作成(APIが描画に使用する画像を管理するビューのこと)
		if (!CreateRenderPass()) return false; // レンダーパスの作成(描画全体のマネージャー。実際に描画に使用するのがサブパス。サブパスを複数個用意することでポストプロセスもできる)
		if (!CreateDepthResources()) return false; // デプステスト用のリソースを生成
		if (!CreateFrameBuffer()) return false; // フレームバッファの作成
		if (!CreateCommandPool()) return false; // コマンドプールを作成(コマンドプールはコマンドバッファを格納するメモリを管理するのに使用する)
		if (!CreateCommandBuffer()) return false; // コマンドバッファの作成
		if (!CreateSyncObjects()) return false; // 同期オブジェクトの作成(各種コマンドの順序を操作するために使用)

		return true;
	}

	void CVulkanAPI::Release()
	{
		// Release Vulkan(作成とは逆の順番で破棄していく)
		CleanupSwapChain();

		// レンダーパスの破棄
		vkDestroyRenderPass(m_LogicalDevice, m_RenderPass, nullptr);

		// 同期オブジェクトの破棄
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(m_LogicalDevice, m_ImageAvailableSemaphones[i], nullptr);
			vkDestroySemaphore(m_LogicalDevice, m_RenderFinishedSemaphores[i], nullptr);
			vkDestroyFence(m_LogicalDevice, m_InFlightFences[i], nullptr);
		}

		// コマンドプールの破棄
		vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);

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

	// Vulkanメインロジック ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// インスタンスを作成
	bool CVulkanAPI::CreateInstance()
	{
		VkResult result = VK_SUCCESS;

		// 使用可レイヤーリストの初期化
		InitAvailableLayerList();

		// デバッグ用の検証レイヤーが使用可かチェックする
		if (m_IsUseDebugValidationLayer && !CheckDebugValidationLayerSupport())
		{
			// 物理デバイスが検証レイヤー機を持っていない場合は機デバッグレイヤーの使用をオフにしておく
			m_IsUseDebugValidationLayer = false;
		}

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
			// レイヤー(Layer) => インスタンスレイヤー => APIをラップして使う感じの拡張機
			// エクステンション(Extension) => 単純にAPIから派生させて生やす感じの拡張機

			//
			InstanceInfo.enabledLayerCount = static_cast<uint32_t>(m_UseLayerList.size()); // 使用する拡張機レイヤーの数
			InstanceInfo.ppEnabledLayerNames = &m_UseLayerList[0]; // 使用する拡張機レイヤーの名前リスト
		
			SetDebugMessengerCreateInfo(debugCreateInfo); // 使用する拡張機本体の設定
			InstanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo; // pNextは拡張機レイヤーの生成情報  
		}
		else
		{
			InstanceInfo.enabledLayerCount = 0;
			InstanceInfo.ppEnabledLayerNames = nullptr;
		}

		// GLFWの拡張機を設定(たぶんここで外部ツールの拡張機を設定できる？)
		auto extensions = GetRequiredExtensions();
		InstanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		InstanceInfo.ppEnabledExtensionNames = &extensions[0];

		// インスタンスを作成
		result = vkCreateInstance(&InstanceInfo, nullptr, &m_Instance);

		return (result == VK_SUCCESS);
	}

	// Vulkanのウィンドウサーフェイスを作成(ウィンドウシステムとやり取りをする箇所)
	bool CVulkanAPI::CreateSurface()
	{
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = glfwGetWin32Window(m_pWindow); // ウィンドウへのハンドル
		createInfo.hinstance = GetModuleHandle(nullptr); // 現在のプロセスへのハンドル

		if (vkCreateWin32SurfaceKHR(m_Instance, &createInfo, nullptr, &m_Surface) != VK_SUCCESS)
		{
			Console::Log("[Error] vkCreateWin32SurfaceKHR\n");
			return false;
		}

		// VulkanのウィンドウサーフェイスとGLFWを結び付ける
		if (glfwCreateWindowSurface(m_Instance, m_pWindow, nullptr, &m_Surface) != VK_SUCCESS)
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

		// 使用したい機を持っている物理デバイスを探す
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
		// 物理デバイスのオプション機を問い合わせる
		// (ベンダー名やグラボ名などの基本的な情報はvkGetPhysicalDevicePropertiesで問い合わせる)
		// vkGetPhysicalDeviceFeaturesでジオメトリシェーダー等の機を処理対象グラボが持っているか確認する

		// デフォルトで有効になっている設定一覧を取得する 
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &supportedFeatures);

		// <<C++構文>> "{ }" のコンストラクターについて
		// コンストラクタが単純な構造体は初期化時に構造体名のコンストラクタを呼ばずに簡潔に中括弧{}のみで初期化することができる.
		// https://learn.microsoft.com/ja-jp/cpp/cpp/initializing-classes-and-structs-without-constructors-cpp?view=msvc-170
		// https://www.ibm.com/docs/ja/zos/2.3.0?topic=initializers-initialization-structures-unions

		// 必要なオプション機を有効にする
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

		// 論理デバイスの拡張機の設定
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

	bool CVulkanAPI::CreateSwapChain()
	{
		// スワップチェーンのサポート状況をチェック
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_PhysicalDevice);

		// 使用できるスワップチェーン情報をチェック
		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.m_Formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.m_PresentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.m_Capabilities);

		// スワップチェーンに含めるイメージの枚数を決める
		uint32_t imageCount = swapChainSupport.m_Capabilities.minImageCount + 1;

		if (swapChainSupport.m_Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.m_Capabilities.maxImageCount)
		{
			// 最大値を超えないようにする
			imageCount = swapChainSupport.m_Capabilities.maxImageCount;
		}

		// スワップチェインイメージの詳細を指定する
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamiryIndices indices = FindQueueFamilies(m_PhysicalDevice);
		uint32_t queueFamilyIndices[] = { indices.m_GraphicsFamily.value(), indices.m_PresentFamily.value() };

		// キューファミリー(キューのマネージャー)が異なる場合の処理を指定する
		if (indices.m_GraphicsFamily != indices.m_PresentFamily)
		{
			// VK_SHARING_MODE_CONCURRENTはスワップチェインイメージを複数のキューファミリ間で共有する(少しパフォーマンスが悪くなる)
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else // キューファミリが同じ場合
		{
			// VK_SHARING_MODE_EXCLUSIVEはスワップチェインイメージを一つのキューファミリだけで使用する指定(パフォーマンスが良い)
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		// イメージが変換可であるかの指定
		createInfo.preTransform = swapChainSupport.m_Capabilities.currentTransform;

		// アルファブレンドを使用するかどうか
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		//
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		// 古いスワップチェインの取り扱いをどうするか
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		// スワップチェインの作成
		if (vkCreateSwapchainKHR(m_LogicalDevice, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain!\n");
		}

		// スワップチェインのイメージのハンドルを取得する
		vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, &m_SwapChainImages[0]);

		//
		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = extent;

		return true;
	}

	// Image => 画像データの本体
	// ImageView => Imageのマネージャー, ラッパー
	bool CVulkanAPI::CreateImageViews()
	{
		m_SwapChainImageViews.resize(m_SwapChainImages.size());

		for (size_t i = 0; i < m_SwapChainImages.size(); i++)
		{
			m_SwapChainImageViews[i] = CreateImageView(m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		}

		return true;
	}

	bool CVulkanAPI::CreateRenderPass()
	{
		// <カラーバッファ> ////////////////////////////////////////////////////////////////
		// レンダーパスの基本的な設定
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = m_SwapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // マルチサンプリング
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // レンダリングの前後にどのような処理を施すか(クリアの方法など)。デプスバッファに適応
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // レンダリング結果をメモリに保存し読み取り可にする。デプスバッファに適応
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // 上記の設定をステンシルバッファに適応。 DONT_CAREは何もしない
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // 上記の設定をステンシルバッファに適応
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // レンダリング前にどのようなレイアウトとして使用するか
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // レンダリング後にどのようなレイアウトとして使用するか

		// サブパスの設定(サブパスとは前のパスのフレームバッファの内容を参照するレンダリング操作。ポストプロセスなどに有用)
		VkAttachmentReference colorAttachmentRef{}; // 前のパスの参照方法の定義(かな？)
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // カラーレンダーバッファとして使用することを明示???

		// <デプスバッファ> ////////////////////////////////////////////////////////////////
		// レンダーパスの基本的な設定
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = FIndDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // マルチサンプリング
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // レンダリングの前後にどのような処理を施すか(クリアの方法など)。デプスバッファに適応
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // レンダリング結果をメモリに保存し読み取り可にする。デプスバッファに適応
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // 上記の設定をステンシルバッファに適応。 DONT_CAREは何もしない
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // 上記の設定をステンシルバッファに適応
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // レンダリング前にどのようなレイアウトとして使用するか
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // レンダリング後にどのようなレイアウトとして使用するか

		// サブパスの設定(サブパスとは前のパスのフレームバッファの内容を参照するレンダリング操作。ポストプロセスなどに有用)
		VkAttachmentReference depthAttachmentRef{}; // 前のパスの参照方法の定義(かな？)
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // カラーレンダーバッファとして使用することを明示???

		///////////////////////////////////////////////////////////////////////////////////
		//
		VkSubpassDescription subpass{}; // 実際に使用するサブパスの設定
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // グラフィック用のサブパスであることを指定する
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef; // 参照方法について
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		// レンダーパスの作成
		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		// サブパスの依存関係を設定(よくわからん)
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_LogicalDevice, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create render pass!\n");
		}

		return true;
	}

	bool CVulkanAPI::CreateDepthResources()
	{
		VkFormat depthFormat = FIndDepthFormat();
		CreateImage(m_SwapChainExtent.width, m_SwapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);

		m_DepthImageView = CreateImageView(m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		return true;
	}

	bool CVulkanAPI::CreateFrameBuffer()
	{
		m_SwapChainFrameBuffers.resize(m_SwapChainImageViews.size());

		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
		{
			std::array<VkImageView, 2> attachments[] = {
				m_SwapChainImageViews[i],
				m_DepthImageView
			};

			VkFramebufferCreateInfo frameBufferInfo{};
			frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferInfo.renderPass = m_RenderPass;
			frameBufferInfo.attachmentCount = static_cast<uint32_t>(attachments->size());
			frameBufferInfo.pAttachments = attachments->data();
			frameBufferInfo.width = m_SwapChainExtent.width;
			frameBufferInfo.height = m_SwapChainExtent.height;
			frameBufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_LogicalDevice, &frameBufferInfo, nullptr, &m_SwapChainFrameBuffers[i]) != VK_SUCCESS)
			{
				return false;
			}
		}

		return true;
	}

	bool CVulkanAPI::CreateCommandPool()
	{
		QueueFamiryIndices queueFamilyIndices = FindQueueFamilies(m_PhysicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.m_GraphicsFamily.value();

		if (vkCreateCommandPool(m_LogicalDevice, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	bool CVulkanAPI::CreateCommandBuffer()
	{
		m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // メイン(プライマリ)コマンドバッファかサブ(セカンダリ)コマンドバッファかの選択
		allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

		// Allocate は確保するという意味
		if (vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	bool CVulkanAPI::CreateSyncObjects()
	{
		//
		m_ImageAvailableSemaphones.resize(MAX_FRAMES_IN_FLIGHT);
		m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		// セマフォの作成(セマフォとはキュー操作の間に順序を追加するためのもの。セマフォの処理はGPUのみで行われる) 
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		// フェンスの作成(フェンスもセマフォと同様の機を持つが、GPUでのコマンドの終了がCPUに知らされるということが違う)
		// セマフォはGOUでの操作の実行順序を指定するために使用され、フェンスはCPUとGPUをお互い同期させるために使用される
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // 初回は前のフレームがないため、このフラグを設定することで最初の呼び出しがすぐに行われるようにする

		// 両者をまとめて作成
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphones[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(m_LogicalDevice, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create semaphores!");
			}
		}

		return true;
	}

	bool CVulkanAPI::BeginRecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		// コマンドバッファの記録開始
		VkCommandBufferBeginInfo beginInfo{}; // 記録に関する詳細な設定
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // このコマンドバッファをどのように使用するか
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &beginInfo) != VK_SUCCESS)
		{
			return false;
		}

		// レンダーパス開始 
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_SwapChainFrameBuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_SwapChainExtent;

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		// レンダーパス開始コマンドを発行
		vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// 動的指定にしたビューポートとシザーの設定をここで行う(ウィンドウのリサイズにとても役立つやつ)
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChainExtent.width);
		viewport.height = static_cast<float>(m_SwapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_CommandBuffers[m_CurrentFrame], 0, 1, &viewport); // ビューポート再設定用のコマンドを発行

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_SwapChainExtent;
		vkCmdSetScissor(m_CommandBuffers[m_CurrentFrame], 0, 1, &scissor); // シザーの再設定用のコマンドを発行

		return true;
	}

	bool CVulkanAPI::EndRecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		// レンダーパス終了
		vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);

		// コマンドバッファの記録を終了
		if (vkEndCommandBuffer(m_CommandBuffers[m_CurrentFrame]) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	bool CVulkanAPI::CleanupSwapChain()
	{
		vkDestroyImageView(m_LogicalDevice, m_DepthImageView, nullptr);
		vkDestroyImage(m_LogicalDevice, m_DepthImage, nullptr);
		vkFreeMemory(m_LogicalDevice, m_DepthImageMemory, nullptr);

		for (size_t i = 0; i < m_SwapChainFrameBuffers.size(); i++)
		{
			vkDestroyFramebuffer(m_LogicalDevice, m_SwapChainFrameBuffers[i], nullptr);
		}

		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
		{
			vkDestroyImageView(m_LogicalDevice, m_SwapChainImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);

		return true;
	}
	
	bool CVulkanAPI::ReCreateSwapChain()
	{
		// ウィンドウが最小化された場合、それが再びアクティブになるまで処理を止める
		int width = 0, height = 0;
		glfwGetFramebufferSize(m_pWindow, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(m_pWindow, &width, &height);
			glfwWaitEvents();
		}

		// 論理デバイスの処理がすべて終わるまで待つ
		vkDeviceWaitIdle(m_LogicalDevice);

		// スワップチェーンの削除と再生成
		CleanupSwapChain();

		CreateSwapChain();
		CreateImageViews();
		CreateDepthResources();
		CreateFrameBuffer();

		return true;
	}

	// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// レイヤー
	void CVulkanAPI::InitAvailableLayerList()
	{
		// レイヤーとは拡張機のこと
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		m_AvailableLayers.resize(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, &m_AvailableLayers[0]);
	}
	
	bool CVulkanAPI::CheckDebugValidationLayerSupport()
	{
		const auto& LayerName = m_UseLayerList[0];

		// 名前を比較して拡張機をサポートしているかチェックする
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
		// コールバックが通知されるメッセージの種類(一般的, 検証用, 性)
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

		// 使用したい物理デバイス拡張機が有効かチェックする
		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		// デバイスが持っているスワップチェーン(画面に示されるのを待っている画像のキューのこと)のキューの拡張機をチェックする
		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.m_Formats.empty() && !swapChainSupport.m_PresentModes.empty();
		}

		// 物理デバイスが持っている機をチェックする(例えば64bit float textureが使えるか, テクスチャ圧縮, マルチビューポート)
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

		// 使用可なキューファミリのリストを取得する
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
	// Presentationとは画像をウィンドウへ描画すること・渡すこと
	// 
	// SwapChainの使用可否の詳細を問い合わせる
	// ウィンドウサーフェイスがSwapChainに対応しているか確認する 
	SwapChainSupportDetails CVulkanAPI::QuerySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;

		// スワップ チェーン内の画像の最小/最大数、画像の最小/最大幅と高さ
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.m_Capabilities);

		// 面形式 (ピクセル形式、色空間)
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.m_Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, &details.m_Formats[0]);
		}

		// 利用可なプレゼンテーション モード
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.m_PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, &details.m_PresentModes[0]);
		}

		return details;
	}

	// SurfaceFormat(色深度)
	VkSurfaceFormatKHR CVulkanAPI::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8_SRGB && availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	// プレゼンテーション モードの決定 (画面に画像を「入れ替える」ための条件)
	VkPresentModeKHR CVulkanAPI::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	// スワップ範囲 (スワップ チェーン内の画像の解像度を決定する)
	VkExtent2D CVulkanAPI::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) // uint32_tの最大値ではないことかチェックする
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width, height;
			glfwGetFramebufferSize(m_pWindow, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	// Buffer
	// 物理デバイス(GPU)のメモリ要件を調べるための関数. つまりグラボが持っているメモリ関連の機をチェックする
	uint32_t CVulkanAPI::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertoes)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			// typeFilterには調べたいメモリの機に関するフラグが入っている
			// 
			if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & propertoes) == propertoes)
			{
				return i;
			}
		}

		return -1;
	}

	void CVulkanAPI::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
		VkDeviceMemory& bufferMemory)
	{
		// バッファの作成
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_LogicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vertex buffer!");
		}

		// バッファに割り当てるメモリオブジェクトを作成する
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_LogicalDevice, buffer, &memRequirements); // メモリ要件の取得

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_LogicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate vertex buffer memory\n");
		}

		// メモリオブジェクトをバッファに割り当てる
		vkBindBufferMemory(m_LogicalDevice, buffer, bufferMemory, 0);
	}

	// 文字通りバッファのコピー
	void CVulkanAPI::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		// コマンドの記録開始 
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		// 実行するコマンドを記録する
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		// コマンドの記録終了
		EndSingleTimeCommands(commandBuffer);
	}

	const std::vector<VkCommandBuffer>& CVulkanAPI::GetCommandBuffers() const
	{
		return m_CommandBuffers;
	}

	// Frame Buffer
	uint32_t CVulkanAPI::GetCurrentFrame() const
	{
		return m_CurrentFrame;
	}

	// Texture
	VkImageView CVulkanAPI::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(m_LogicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}

	bool CVulkanAPI::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
	{
		// テクスチャイメージを生成
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // マルチサンプリングに関連
		imageInfo.flags = 0;

		if (vkCreateImage(m_LogicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
		{
			return false;
		}

		// テクスチャイメージにメモリを割り当てる
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_LogicalDevice, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		
		uint32_t MemoryType = FindMemoryType(memRequirements.memoryTypeBits, properties);
		if (MemoryType == -1) return false;
		allocInfo.memoryTypeIndex = MemoryType;

		if (vkAllocateMemory(m_LogicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		{
			return false;
		}

		vkBindImageMemory(m_LogicalDevice, image, imageMemory, 0);

		return true;
	}

	void CVulkanAPI::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		// コマンド記録開始
		VkCommandBuffer comandBuffer = BeginSingleTimeCommands();

		// イメージの形式を別形式に変換する(イメージテクスチャ用のメモリバリア)
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			comandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		// コマンド記録終了
		EndSingleTimeCommands(comandBuffer);
	}

	void CVulkanAPI::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		//
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		//
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0 ,0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		//
		EndSingleTimeCommands(commandBuffer);
	}

	// Depth
	VkFormat CVulkanAPI::FIndDepthFormat()
	{
		return FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	VkFormat CVulkanAPI::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!\n");
	}

	bool CVulkanAPI::HasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	// Command
	VkCommandBuffer CVulkanAPI::BeginSingleTimeCommands()
	{
		// コマンドを記録 
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void CVulkanAPI::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		// コマンドを実行
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_GraphicsQueue);

		vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &commandBuffer);
	}

	// ループ中の描画関連処理 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CVulkanAPI::BeginRender(ERenderPassType RenderPassType)
	{
		// 前のフレーム処理が終わるのを待つ
		vkWaitForFences(m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		// スワップチェーンからイメージを取得する
		// m_ImageAvailableSemaphoneのセマフォでGPU側の処理を止める
		// 現在、スワップチェーンが古くないかチェックする(最新のウィンドウではサイズ等が変わっているかも!!)
		VkResult result = vkAcquireNextImageKHR(m_LogicalDevice, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphones[m_CurrentFrame], VK_NULL_HANDLE, &m_CurrentImageIndex);
		
		// VK_ERROR_OUT_OF_DATE_KHR: スワップ チェーンはサーフェスと互換性がなくなり、レンダリングに使用できなくなりました(ウィンドウサイズの変更)
		// VK_SUBOPTIMAL_KHR: スワップ チェーンを使用してサーフェスに正常に示することはできますが、サーフェス プロパティは正確に一致しなくなりました。
		m_IsReCreateSwapChain = false;
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			ReCreateSwapChain(); // 最新ではなのでスワップチェーンを作り直す

			m_IsReCreateSwapChain = true;
			return true;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			return false;
		}

		// 処理が終わって次の処理に移るのでフェンスをまた使える状態にシグナルをリセットしておく
		vkResetFences(m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame]);

		// ユニフォームデータの更新(各3Dオブジェクト固有)
		//UpdateUniformBuffer(m_CurrentFrame);

		// コマンドバッファの記録
		vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0); // まずリセット

		// 記録スタート
		m_WaitRendering = false;
		if (!BeginRecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], m_CurrentImageIndex)) return false;

		return true;
	}

	bool CVulkanAPI::EndRender()
	{
		m_WaitRendering = true;

		// スワップチェーンを作り直しているので1フレーム待つ
		if (m_IsReCreateSwapChain) return true;

		// 記録終了
		if (!EndRecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], m_CurrentImageIndex)) return false;

		//
		// コマンドバッファの送信(たぶんここで描画される？)
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphore[] = { m_ImageAvailableSemaphones[m_CurrentFrame] }; // 画像に色が書き込まれて利用可になるまで待つセマフォ
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphore;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];

		VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] }; // コマンドの実行が終了したことを知らせるセマフォ
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// コマンドバッファをグラフィックキューに送信
		// コマンドバッファにはコマンドが入っていてそのコマンドをキューが実行する
		// キューはタスクでその具体的なタスク内容がコマンドという理解もできる
		if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
		{
			return false;
		}

		// プレゼンテーション(結果をスワップチェーンに送信して最終結果を画面に示する)
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		// イメージを示するスワップチェーンを選択
		VkSwapchainKHR swapChains[] = { m_SwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_CurrentImageIndex;

		presentInfo.pResults = nullptr;

		// プレゼンテーションキューを実行
		VkResult result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

		// 可な限り最良な結果を得るために念のためもう一度最新かチェックする
		// VK_ERROR_OUT_OF_DATE_KHR: スワップ チェーンはサーフェスと互換性がなくなり、レンダリングに使用できなくなりました(ウィンドウサイズの変更)
		// VK_SUBOPTIMAL_KHR: スワップ チェーンを使用してサーフェスに正常に示することはできますが、サーフェス プロパティは正確に一致しなくなりました。
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized)
		{
			m_FramebufferResized = false;
			ReCreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		// 現在処理するフレームを更新する
		m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		return true;
	}

	bool CVulkanAPI::IsWaitting()
	{
		return m_IsReCreateSwapChain || m_WaitRendering;
	}

	// Device
	const VkPhysicalDevice& CVulkanAPI::GetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}

	const VkDevice& CVulkanAPI::GetLogicalDevice() const
	{
		return m_LogicalDevice;
	}

	// SwapChain/Image
	const VkExtent2D& CVulkanAPI::GetSwapChainExtent() const
	{
		return m_SwapChainExtent;
	}

	// Rendering
	const VkRenderPass& CVulkanAPI::GetRenderPass() const
	{
		return m_RenderPass;
	}
}

#endif // !__DAWN__