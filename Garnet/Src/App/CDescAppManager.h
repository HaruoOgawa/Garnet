#pragma once

#if (defined(USE_VULKAN) || defined(USE_WEBGPU)) && !defined(__EMSCRIPTEN__)

#ifdef USE_VULKAN
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#endif // USE_VULKAN

#include <glfw3.h>
#include <glfw3native.h>

#include <memory>
#include "EAppType.h"

#ifdef USE_WEBGPU
namespace api { class CWebGPUAPI; }
#else
namespace api { class CVulkanAPI; }
#endif
namespace app{ class IApp; }

namespace input { class CInputState; }

namespace descapp
{
	class CDescAppManager
	{
		GLFWwindow* m_pWindow;
#ifdef USE_WEBGPU
		std::shared_ptr<api::CWebGPUAPI> m_GraphicsAPI;
#else
		std::shared_ptr<api::CVulkanAPI> m_GraphicsAPI;
#endif
		std::shared_ptr<app::IApp> m_App;
#ifdef USE_INPUT_SYSTEM
		std::shared_ptr<input::CInputState> m_InputState;
#endif
		const int WIDTH = 1920;
		const int HEIGHT = 1080;

		bool m_IsRunLoop;

		float m_SecondsTime;
		float m_DeltaSecondsTime;

	private:
		bool InitWindow();

		bool Release();

		bool Update();
		bool Draw();
	public:
		CDescAppManager(app::EAppType AppType);
		virtual ~CDescAppManager();
		bool Initialize();
		bool RunLopp();

		bool IsRunLoop() { return m_IsRunLoop; }
#ifdef USE_INPUT_SYSTEM
		const std::shared_ptr<input::CInputState>& GetInputState()const { return m_InputState; }
#endif
		void ResizeWindow(int w, int h);
	};
}
#endif