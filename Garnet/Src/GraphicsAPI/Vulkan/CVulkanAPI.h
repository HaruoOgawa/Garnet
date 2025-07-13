#pragma once
#ifdef USE_VULKAN
#include <memory>
#include <vector>
#include <optional>
#include <set>
#include <algorithm>
#include <array>
#include <map>

#include "../CGraphicsAPI.h"
#include "../../Message/Console.h"

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

namespace api
{
	class CVulkanRenderPass;

#define VK_CHECK_RESULT(f) \
	{\
		VkResult res = (f);\
		if (res != VK_SUCCESS)\
		{\
			Console::Log("[Error] Vulkan Status : %s\n", string_VkResult(res));\
			assert(res == VK_SUCCESS);\
		}\
	}\

	struct QueueFamiryIndices
	{
		// GPGPUだけのQueueもあるが、今回はGraphicsとの同期が必要なため、GraphicsとComputeの両方のBitに対応しているQueueを取得する
		std::optional<uint32_t> m_GraphicsAndComputeFamily;
		std::optional<uint32_t> m_PresentFamily;

		bool IsComplete() {
			return m_GraphicsAndComputeFamily.has_value() && m_PresentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR m_Capabilities;
		std::vector<VkSurfaceFormatKHR> m_Formats;
		std::vector<VkPresentModeKHR> m_PresentModes;
	};

	class CVulkanAPI : public CGraphicsAPI
	{
		//
		const std::string m_VertexShaderExtension;
		const std::string m_FragmentShaderExtension;
		const std::string m_GeometryShaderExtension;
		const std::string m_HullShaderExtension;
		const std::string m_DomainShaderExtension;
		const std::string m_ComputeShaderExtension;

		int m_Width;
		int m_Height;
		
		// GLFW Window
		GLFWwindow* m_pWindow;

		// Layer
#ifdef _DEBUG
		bool m_IsUseDebugValidationLayer = true;
#else
		bool m_IsUseDebugValidationLayer = false;
#endif // _DEBUG

		const std::vector<const char*> m_UseLayerList = {
			"VK_LAYER_KHRONOS_validation"
		};
		std::vector<VkLayerProperties> m_AvailableLayers;

		VkDebugUtilsMessengerEXT m_DebugMessenger;

		// Instance
		VkInstance m_Instance;

		// Window Surface
		VkSurfaceKHR m_Surface;

		// Device
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_LogicalDevice;
		const std::vector<const char*> m_DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef RUNTIME_SHADER_EDITING
			// ToDo: ShaderObject拡張が有効になっているとRenderDocでデバッグができなくなる
			VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
#endif // RUNTIME_SHADER_EDITING
		};

		// Queue
		VkQueue m_GraphicsQueue;
		VkQueue m_ComputeQueue;
		VkQueue m_PresentQueue;

		// SwapChain/Image
		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainColorImageFormat;
		VkFormat m_SwapChainDepthImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImageView> m_SwapChainImageViews;

		uint32_t m_CurrentImageIndex;
		//bool m_IsReCreateSwapChain;

		// Rendering
		VkRenderPass m_SwapChainRenderPass;
		VkRenderPass m_CurrentRenderPass;
		CVulkanRenderPass* m_pCurrentVulkanRenderPass;

		// Depth Test
		VkImage m_SwapChainDepthImage;
		VkDeviceMemory m_SwapChainDepthImageMemory;
		VkImageView m_SwapChainDepthImageView;

		// Frame Buffer
		const int MAX_FRAMES_IN_FLIGHT = 3;
		std::vector<VkFramebuffer> m_SwapChainFrameBuffers;
		uint32_t m_CurrentFrame = 0;

		// Command Buffer
		VkCommandPool m_CommandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		// Sync Obj
		std::vector<VkSemaphore> m_ImageAvailableSemaphones;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkSemaphore> m_ComputeFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;
		std::vector<VkFence> m_ComputeInFlightFences;

		bool m_FramebufferResized = false;
	private:
		// Vulkanメインロジック ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateInstance();
		bool CreateSurface();
		bool CreateDevicesWithQueues();
		bool CreateSwapChain();
		bool CreateImageViews();
		bool CreateSwapChainRenderPass();
		bool CreateSwapChainDepthResources();
		bool CreateSwapChainFrameBuffer();
		bool CreateCommandPool();
		bool CreateCommandBuffer();
		bool CreateSyncObjects();

		bool BeginRenderPass(uint32_t imageIndex);
		bool EndRenderPass();

		bool CleanupSwapChain();
		bool ReCreateSwapChain();

		// ヘルパー関数 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Layer
		void InitAvailableLayerList();
		bool CheckDebugValidationLayerSupport();
		void SetDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageServerity,
			VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		std::vector<const char*> GetRequiredExtensions();
		bool SetupDebugMessengerEXT();
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

		// Device
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		// Presentation
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availablePresentModes);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentMode);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		// Buffer
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertoes);
		
	public:
		CVulkanAPI(int Width, int Height);
		virtual ~CVulkanAPI();

		virtual bool Initialize(window::IWindowAPI* pWindowAPI) override;

		virtual void Release() override;

		virtual bool CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, int Width = -1, int Height = -1,
			const graphics::SRenderPassState& PassState = graphics::SRenderPassState(1)) override;
		virtual std::shared_ptr<graphics::CVertexBuffer> CreateVertexBuffer() override;
		virtual std::shared_ptr<graphics::CIndexBuffer> CreateIndexBuffer() override;
		virtual std::shared_ptr<graphics::IRenderer> CreateRenderer() override;
		virtual std::shared_ptr<graphics::CMaterial> CreateMaterial(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, graphics::ECullMode CullMode) override;
		virtual std::shared_ptr<graphics::CTexture> CreateTexture(bool UseMipMap = false, graphics::STextureSamplerParam SamplerParam = {}) override;
#ifdef USE_GPGPU
		virtual std::shared_ptr<api::IGPGPUHandler> CreateGPGPUHandler(const std::shared_ptr<graphics::CMaterial>& ComputeMaterial) override;
#endif // USE_GPGPU

#ifdef USE_RTXGI
		virtual std::shared_ptr<graphics::CRTXGIController> CreateRTXGIController() override;
#endif

		virtual bool Resize(int Width, int Height) override;

		virtual bool PrepareRender() override;
		virtual bool BeginRender(const std::string& PassName = "") override;
		virtual bool EndRender() override;
		virtual bool SubmitRender() override;

		virtual std::string GetVertexShaderExtension() const override;
		virtual std::string GetFragmentShaderExtension() const override;
		virtual std::string GetGeometryShaderExtension() const override;
		virtual std::string GetHullShaderExtension() const override;
		virtual std::string GetDomainShaderExtension() const override;
		virtual std::string GetComputeShaderExtension() const override;

		virtual int GetWidth() const override;
		virtual int GetHeight() const override;

		virtual std::shared_ptr<graphics::IRenderPass> FindOffScreenRenderPass(const std::string& PassName) override;
		virtual bool CopyColorBuffer(const std::string& SrcPassName, const std::string& DstPassName) override;
		virtual bool CopyDepthBuffer(const std::string& SrcPassName, const std::string& DstPassName) override;

		VkRenderPass GetSwapChainRenderPass() const;

		virtual bool IsEnabledRuntimeShaderEditing() const override;

		virtual bool CheckValidShader(std::string& ErrorMsg, const std::vector<unsigned char>& ShaderCode, graphics::EShaderStage ShaderStage) override;

		//
		int GetMaxFramesInFlight() const { return MAX_FRAMES_IN_FLIGHT; }

		// Instance
		const VkInstance& GetInstance() const;

		// Device
		const VkPhysicalDevice& GetPhysicalDevice() const;
		const VkDevice& GetLogicalDevice() const;

		// SwapChain/Image
		const VkExtent2D& GetSwapChainExtent() const;

		// Rendering
		const VkRenderPass& GetCurrentRenderPass() const;

		VkFormat FindImageFormat(api::ERenderPassFormat RenderPassFormat, bool& UseColor, bool& UseDepth, bool& UseStencil) const;
		VkImageUsageFlags FindImageUsage(api::ERenderPassFormat RenderPassFormat, bool ReadOnShader) const;

		VkSampleCountFlagBits GetMSAASampleFormat(int AASampleNum) const;
		VkSampleCountFlagBits GetMSAAMaxUsableSampleCount() const;

		// Command
		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		bool CreateCommandPool(VkCommandPool& CommandPool);
		bool CreateCommandBuffer(VkCommandBuffer& CommandBuffer, VkCommandPool CommandPool);

		// Queue
		QueueFamiryIndices FindQueueFamilies(VkPhysicalDevice device);
		VkQueue GetGraphicsQueue()const { return m_GraphicsQueue; }
		VkQueue GetComputeQueue()const { return m_ComputeQueue; }
		VkQueue GetPresentQueue() const { return m_PresentQueue; }

		// Sync
		VkSemaphore GetRenderFlightSemaphore()const { return m_RenderFinishedSemaphores[m_CurrentFrame]; }
		VkSemaphore GetComputeFlightSemaphore()const { return m_ComputeFinishedSemaphores[m_CurrentFrame]; }
		VkFence GetInFlightFence()const { return m_InFlightFences[m_CurrentFrame]; }
		VkFence GetComputeInFlightFence()const { return m_ComputeInFlightFences[m_CurrentFrame]; }

		// Texture
		VkImageView CreateImageView(VkImage image, VkFormat format, graphics::ETextureType TextureType, float MipCount, 
			bool UseMipMap, bool UseColor, bool UseDepth, bool UseStencil);
		bool CreateImage(uint32_t width, uint32_t height, VkSampleCountFlagBits msaaSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
			VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, graphics::ETextureType TextureType, float MipCount, bool UseMipMap);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, float MipCount, bool UseMipMap);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, graphics::ETextureType TextureType, float MipCount, bool UseMipMap, bool HasMipData);

		// Buffer
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		VkCommandBuffer GetCurrentCommandBuffer() const;
		const std::vector<VkCommandBuffer>& GetCommandBuffers() const;

		bool BeginRecordCommandBuffer();
		bool EndRecordCommandBuffer();

		// Frame Buffer
		uint32_t GetCurrentFrame() const;

		// Extension /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool LoadExtensions();
		
		PFN_vkCmdBindShadersEXT BindShadersEXT;
		PFN_vkCreateShadersEXT CreateShadersEXT;
		PFN_vkDestroyShaderEXT DestroyShaderEXT;
		PFN_vkCmdSetVertexInputEXT SetVertexInputEXT;
		PFN_vkCmdSetPrimitiveTopologyEXT SetPrimitiveTopologyEXT;
		PFN_vkCmdSetPrimitiveRestartEnableEXT SetPrimitiveRestartEnableEXT;
		PFN_vkCmdSetViewportWithCountEXT SetViewportWithCountEXT;
		PFN_vkCmdSetScissorWithCountEXT SetScissorWithCountEXT;
		PFN_vkCmdSetCullModeEXT SetCullModeEXT;
		PFN_vkCmdSetFrontFaceEXT SetFrontFaceEXT;
		PFN_vkCmdSetRasterizerDiscardEnableEXT SetRasterizerDiscardEnableEXT;
		PFN_vkCmdSetPolygonModeEXT SetPolygonModeEXT;
		PFN_vkCmdSetRasterizationSamplesEXT SetRasterizationSamplesEXT;
		PFN_vkCmdSetAlphaToCoverageEnableEXT SetAlphaToCoverageEnableEXT;
		PFN_vkCmdSetDepthTestEnableEXT SetDepthTestEnableEXT;
		PFN_vkCmdSetDepthWriteEnableEXT SetDepthWriteEnableEXT;
		PFN_vkCmdSetDepthCompareOpEXT SetDepthCompareOpEXT;
		PFN_vkCmdSetDepthBiasEnableEXT SetDepthBiasEnableEXT;
		PFN_vkCmdSetStencilTestEnableEXT SetStencilTestEnableEXT;
		PFN_vkCmdSetSampleMaskEXT SetSampleMaskEXT;
		PFN_vkCmdSetColorBlendEnableEXT SetColorBlendEnableEXT;
		PFN_vkCmdSetColorWriteMaskEXT SetColorWriteMaskEXT;
	};
}
#endif