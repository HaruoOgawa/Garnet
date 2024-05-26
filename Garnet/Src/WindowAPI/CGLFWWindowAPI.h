#pragma once

#include "../Interface/IWindowAPI.h"

#ifdef USE_GLFW
#include <glfw3.h>
#include <glfw3native.h>

#include <memory>

namespace app{ class CAppCore; }

namespace window
{
	class CGLFWWindowAPI : public IWindowAPI
	{
		app::CAppCore* m_pCAppCore;

		GLFWwindow* m_pWindow;
	private:
		bool InitWindow(int Width, int Height);
	public:
		CGLFWWindowAPI();
		virtual ~CGLFWWindowAPI() = default;

		virtual GLFWwindow* GetGLFWWindow() const override;
		
		virtual bool Release() override;

		virtual bool Initialize(app::CAppCore* pAppCore, app::SAppSettings Settings) override;
		
		virtual void SwapWindowBuffers() override;

		virtual void AssignCurrentWindowSize() override;

		virtual void PollEvents() override;

		virtual app::CAppCore* GetAppCore() const override;

		virtual void ResizeWindow(int w, int h) override;

		// インプットイベント
		virtual void OnKeyDown(std::string key) override;
		virtual void OnKeyUp(std::string key) override;

		// リサイズイベント
		virtual void OnResize(int w, int h) override;

		// フォーカスイベント
		virtual void OnFocus(int focused) override;

		// マウスイベント
		virtual void OnMouseDown(int buttonNum, int x, int y) override;
		virtual void OnMouseUp(int buttonNum, int x, int y) override;
		virtual void OnMouseMove(int x, int y) override;
		virtual void OnMouseWheel(int deltaY) override;
	};
}
#endif