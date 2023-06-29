#pragma once
#ifndef __DAWN__
#include <memory>
#include <vector>
#include <optional>
#include <set>
#include <algorithm>
#include <array>
#include <unordered_map>

#include "../../Interface/IGraphicsAPI.h"

namespace api
{
	class CVulkanRenderPass;

	struct QueueFamiryIndices
	{
		std::optional<uint32_t> m_GraphicsFamily;
		std::optional<uint32_t> m_PresentFamily;

		bool IsComplete() {
			return m_GraphicsFamily.has_value() && m_PresentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR m_Capabilities;
		std::vector<VkSurfaceFormatKHR> m_Formats;
		std::vector<VkPresentModeKHR> m_PresentModes;
	};

	class CVulkanAPI : public IGraphicsAPI
	{
		//
		const std::string m_ShaderExtension;
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
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// Queue
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		// SwapChain/Image
		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImageView> m_SwapChainImageViews;

		uint32_t m_CurrentImageIndex;
		//bool m_IsReCreateSwapChain;

		// Rendering
		std::unordered_map<std::string, std::shared_ptr<CVulkanRenderPass>> m_RenderPassMap;
		VkRenderPass m_RenderPass;

		// Depth Test
		VkImage m_DepthImage;
		VkDeviceMemory m_DepthImageMemory;
		VkImageView m_DepthImageView;

		// Frame Buffer
		const int MAX_FRAMES_IN_FLIGHT = 2;
		std::vector<VkFramebuffer> m_SwapChainFrameBuffers;
		uint32_t m_CurrentFrame = 0;

		// Command Buffer
		VkCommandPool m_CommandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		// Sync Obj
		std::vector<VkSemaphore> m_ImageAvailableSemaphones;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;

		bool m_FramebufferResized = false;
	private:
		// Vulkanメインロジック ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateInstance();
		bool CreateSurface();
		bool CreateDevices();
		bool CreateSwapChain();
		bool CreateImageViews();
		bool CreateRenderPass();
		bool CreateDepthResources();
		bool CreateFrameBuffer();
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

		// Queue
		QueueFamiryIndices FindQueueFamilies(VkPhysicalDevice device);

		// Presentation
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availablePresentModes);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentMode);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		// Buffer
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertoes);
		
		// Depth
		VkFormat FIndDepthFormat();
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		bool	 HasStencilComponent(VkFormat format);
	public:
		CVulkanAPI(int Width, int Height);
		virtual ~CVulkanAPI();

		virtual bool InitializeWithGLFW(GLFWwindow* pWindow) override;
		void Release();

		virtual bool CreateRenderPass(const std::string& PassName, int Width, int Height, ERenderPassFormat RenderPassFormat) override;
		virtual std::shared_ptr<renderer::IRenderer> CreateRenderer() override;
		virtual std::shared_ptr<graphics::CMaterial> CreateMaterial() override;
		virtual std::shared_ptr<graphics::CTexture> CreateTexture(bool UseMipMap = false) override;

		virtual bool Resize(int Width, int Height) override;

		virtual bool BeginRender(const std::string& PassName = "") override;
		bool EndRender() override;

		virtual const std::string& GetShaderExtension() const override;

		//
		int GetMaxFramesInFlight() const { return MAX_FRAMES_IN_FLIGHT; }

		// Device
		const VkPhysicalDevice& GetPhysicalDevice() const;
		const VkDevice& GetLogicalDevice() const;

		// SwapChain/Image
		const VkExtent2D& GetSwapChainExtent() const;

		// Rendering
		const VkRenderPass& GetRenderPass() const;

		// Command
		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		// Texture
		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, graphics::ETextureType TextureType, float MipCount, bool UseMipMap);
		bool CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
			VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, graphics::ETextureType TextureType, float MipCount, bool UseMipMap);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, float MipCount, bool UseMipMap);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, graphics::ETextureType TextureType, float MipCount, bool UseMipMap, bool HasMipData);

		// Buffer
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		const std::vector<VkCommandBuffer>& GetCommandBuffers() const;
		bool BeginRecordCommandBuffer();
		bool EndRecordCommandBuffer();
		bool SubmitCommandNoSemaphore();

		// Frame Buffer
		uint32_t GetCurrentFrame() const;
	};
}
#endif