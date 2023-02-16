#pragma once
#include <memory>
#include <vector>
#include <optional>
#include <set>

#include "../Interface/IGraphicsAPI.h"

#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

namespace api
{
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
		// Layer
#ifdef _DEBUG
		const bool m_IsUseDebugValidationLayer = true;
#else
		const bool m_IsUseDebugValidationLayer = false;
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

	private:
		// 初期化関連の関数 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateInstance();
		bool CreateSurface(GLFWwindow* pWindow);
		bool CreateDevices();

		// ヘルパー関数 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		// レイヤー
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

		// デバイス
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		// Queue
		QueueFamiryIndices FindQueueFamilies(VkPhysicalDevice device);

		// Presentation
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	public:
		CVulkanAPI();
		virtual ~CVulkanAPI();

		bool Initialize(GLFWwindow* pWindow);
		void Release();

		std::shared_ptr<renderer::IRenderer> CreateRenderer() override;
	};
}