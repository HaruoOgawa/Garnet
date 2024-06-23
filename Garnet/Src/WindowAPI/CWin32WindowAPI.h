#pragma once

#include "../Interface/IWindowAPI.h"

#ifdef USE_WIN32_WindowAPI
#include <memory>
#include <Windows.h>
#include "../GraphicsAPI/OpenGL/wglDef.h"

namespace app { class CAppCore; }

namespace window
{
	class CWin32WindowAPI : public IWindowAPI
	{
		app::CAppCore* m_pCAppCore;

		HWND  m_Window; // ハンドルウィンドウ
		HDC   m_Device_Context; // デバイスのコンテキスト
		HGLRC m_Rendering_Context; // OpenGLのコンテキスト
	private:
		bool InitWindow(HINSTANCE hInstance, app::SAppSettings Settings);
		bool InitGLContext();
	public:
		CWin32WindowAPI();
		virtual ~CWin32WindowAPI() = default;

		virtual const HWND& GetWin32Window() const override;

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
#endif // USE_WIN32_WindowAPI