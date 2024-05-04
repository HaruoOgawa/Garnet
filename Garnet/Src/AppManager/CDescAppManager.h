#pragma once

#include "../Interface/IWindowAPI.h"

#ifdef USE_GLFW

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
#elif USE_VULKAN
namespace api { class CVulkanAPI; }
#elif USE_OPENGL
namespace api { class COpenGLAPI; }
#endif
namespace app{ class CAppCore; }

namespace input { class CInputState; }

namespace descapp
{
	class CDescAppManager : public IWindowAPI
	{
		GLFWwindow* m_pWindow;
#ifdef USE_WEBGPU
		std::shared_ptr<api::CWebGPUAPI> m_GraphicsAPI;
#elif USE_VULKAN
		std::shared_ptr<api::CVulkanAPI> m_GraphicsAPI;
#elif USE_OPENGL
		std::shared_ptr<api::COpenGLAPI> m_GraphicsAPI;
#endif
		std::shared_ptr<app::CAppCore> m_AppCore;
		std::shared_ptr<input::CInputState> m_InputState;
		const int WIDTH = 1920;
		const int HEIGHT = 1080;

		bool m_IsRunLoop;

		float m_SecondsTime;
		float m_DeltaSecondsTime;

	private:
		bool InitWindow();

		bool Release();

		bool Update();
		bool LateUpdate();
		bool FixedUpdate();
		bool Draw();
	public:
		CDescAppManager(app::EAppType AppType);
		virtual ~CDescAppManager();

		virtual GLFWwindow* GetGLFWWindow() const override;

		const std::shared_ptr<app::CAppCore>& GetAppCore() const;
		
		bool Initialize();
		bool RunLopp();

		bool IsRunLoop() { return m_IsRunLoop; }
		const std::shared_ptr<input::CInputState>& GetInputState()const { return m_InputState; }
		void ResizeWindow(int w, int h);
	};
}
#endif