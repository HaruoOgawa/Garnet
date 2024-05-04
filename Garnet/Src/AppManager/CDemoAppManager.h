#pragma once

#include "../Interface/IWindowAPI.h"

#ifdef USE_WIN32_WindowAPI
#include <memory>
#include <Windows.h>
//#include "../GraphicsAPI/OpenGL/wglDef.h"
#include "EAppType.h"

namespace app { class CAppCore; }

namespace app
{
	class CDemoAppManager : public IWindowAPI
	{
		HWND  m_Window; // ハンドルウィンドウ
		RECT  m_WorkArea; // スクリーンサイズ
		HDC   m_Device_Context; // デバイスのコンテキスト
		HGLRC m_Rendering_Context; // OpenGLのコンテキスト

		const int WIDTH = 1920;
		const int HEIGHT = 1080;

		bool m_IsRunLoop;

		float m_SecondsTime;
		float m_DeltaSecondsTime;
		
		std::shared_ptr<app::CAppCore> m_AppCore;

	private:
		bool InitWindow(HINSTANCE hInstance);
		bool InitGLContext();

		bool Update();
		bool LateUpdate();
		bool FixedUpdate();
		bool Draw();
	public:
		CDemoAppManager(app::EAppType AppType);
		virtual ~CDemoAppManager();

		virtual const HWND& GetWin32Window() const override;

		const std::shared_ptr<app::CAppCore>& GetAppCore() const;

		bool Initialize(HINSTANCE hInstance);
		bool RunLopp();

		bool IsRunLoop() { return m_IsRunLoop; }
		void SetRunLoop(bool RunLoop) { m_IsRunLoop = RunLoop; }
		void ResizeWindow(int w, int h);
	};
}
#endif // USE_WIN32_WindowAPI