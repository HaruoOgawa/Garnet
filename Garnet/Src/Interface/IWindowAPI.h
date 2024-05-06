#pragma once

#include <string>

#ifdef USE_GLFW
#include <glfw3.h>
#include <glfw3native.h>
#elif USE_WIN32_WindowAPI
#include <Windows.h>
#endif

namespace app { class CAppCore; }

namespace window
{
	class IWindowAPI
	{
	public:
#ifdef USE_GLFW
		virtual GLFWwindow* GetGLFWWindow() const = 0;
#elif USE_WIN32_WindowAPI
		virtual const HWND& GetWin32Window() const = 0;
#endif
		virtual bool Release() = 0;

		virtual bool Initialize(app::CAppCore* pAppCore, int Width, int Height) = 0;

		virtual void SwapWindowBuffers() = 0;

		virtual void AssignCurrentWindowSize() = 0;

		virtual void PollEvents() = 0;

		virtual app::CAppCore* GetAppCore() const = 0;

		virtual void ResizeWindow(int w, int h) = 0;

		// インプットイベント
		virtual void OnKeyDown(std::string key) = 0;
		virtual void OnKeyUp(std::string key) = 0;

		// リサイズイベント
		virtual void OnResize(int w, int h) = 0;

		// フォーカスイベント
		virtual void OnFocus(int focused) = 0;

		// マウスイベント
		virtual void OnMouseDown(int buttonNum, int x, int y) = 0;
		virtual void OnMouseUp(int buttonNum, int x, int y) = 0;
		virtual void OnMouseMove(int x, int y) = 0;
		virtual void OnMouseWheel(int deltaY) = 0;
	};
}