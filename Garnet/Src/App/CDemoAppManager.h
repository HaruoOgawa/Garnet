#pragma once

#ifdef USE_OPENGL
#include <memory>
#include <Windows.h>
//#include "../GraphicsAPI/OpenGL/wglDef.h"
#include "EAppType.h"

namespace api { class COpenGLAPI; }
namespace app { class IApp; }
namespace input { class CInputState; }
namespace resource { class CLoadWorker; }
namespace gui { class IGUIEngine; }

namespace app
{
	class CDemoAppManager
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

		std::shared_ptr<resource::CLoadWorker> m_LoadWorker;

		std::shared_ptr<api::COpenGLAPI> m_GraphicsAPI;
		std::shared_ptr<app::IApp> m_App;

		std::shared_ptr<input::CInputState> m_InputState;

		std::shared_ptr<gui::IGUIEngine> m_GUIEngine;
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

		bool Initialize(HINSTANCE hInstance);
		bool RunLopp();

		bool IsRunLoop() { return m_IsRunLoop; }
		void SetRunLoop(bool RunLoop) { m_IsRunLoop = RunLoop; }
		const std::shared_ptr<input::CInputState>& GetInputState()const { return m_InputState; }
		void ResizeWindow(int w, int h);
	};
}
#endif // USE_OPENGL