#pragma once

#ifdef USE_OPENGL
#include <glfw3.h>
#include <glfw3native.h>

#include <memory>
#include "EAppType.h"

namespace app
{
	class CDemoAppManager
	{
		GLFWwindow* m_pWindow;
		const int WIDTH = 1920;
		const int HEIGHT = 1080;

		bool m_IsRunLoop;
	private:
		bool InitWindow();
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