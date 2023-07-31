#pragma once

#ifdef USE_OPENGL
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>
#include <glfw3native.h>

#include <memory>
#include "EAppType.h"

namespace api { class COpenGLAPI; }
namespace app { class IApp; }

namespace app
{
	class CDemoAppManager
	{
		GLFWwindow* m_pWindow;
		const int WIDTH = 1920;
		const int HEIGHT = 1080;

		bool m_IsRunLoop;

		float m_SecondsTime;
		float m_DeltaSecondsTime;

		std::shared_ptr<api::COpenGLAPI> m_GraphicsAPI;
		std::shared_ptr<app::IApp> m_App;
	private:
		bool InitWindow();

		bool Update();
		bool Draw();
	public:
		CDemoAppManager(app::EAppType AppType);
		virtual ~CDemoAppManager();

		bool Initialize();
		bool RunLopp();

		bool IsRunLoop() { return m_IsRunLoop; }
		void SetRunLoop(bool RunLoop) { m_IsRunLoop = RunLoop; }

		void ResizeWindow(int w, int h);
	};
}
#endif // USE_OPENGL