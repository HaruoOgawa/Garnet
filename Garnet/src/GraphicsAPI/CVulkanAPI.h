#pragma once
#include <memory>
#include <vector>

#include "../Interface/IGraphicsAPI.h"

#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

namespace api
{
	class CVulkanAPI : public IGraphicsAPI
	{
		// レイヤー
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

		// インスタンス
		VkInstance m_Instance;

		// Window Surface
		VkSurfaceKHR m_Surface;
	private:
		// 初期化関連の関数
		bool CreateInstance();
		bool CreateSurface(GLFWwindow* pWindow);

		// ヘルパー関数 ////////////////////////////////////////////////////
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
	public:
		CVulkanAPI();
		virtual ~CVulkanAPI();

		bool Initialize(GLFWwindow* pWindow);
		void Release();

		std::shared_ptr<renderer::IRenderer> CreateRenderer() override;
	};
}