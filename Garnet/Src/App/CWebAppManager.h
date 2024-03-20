#pragma once
#ifdef __EMSCRIPTEN__
#include <string>
#include <memory>
#include "EAppType.h"

namespace api { class CWebGPUAPI; }
namespace app { class IApp; }
namespace input { class CInputState; }
namespace resource { class CLoadWorker; }

namespace webapp
{
	class CWebAppManager
	{
		bool m_IsRunLoop;
		std::shared_ptr<api::CWebGPUAPI> m_GraphicsAPI;
		std::shared_ptr<app::IApp> m_App;
		
		std::shared_ptr<input::CInputState> m_InputState;

		float m_SecondsTime;
		float m_DeltaSecondsTime;

		std::shared_ptr<resource::CLoadWorker> m_LoadWorker;

		int m_Width;
		int m_Height;

	private:
		bool Release();

		bool Update();
		bool LateUpdate();
		bool FixedUpdate();
		bool Draw();

		void KeyAction(std::string key, bool IsDown);
	public:
		CWebAppManager(app::EAppType AppType, int Width, int Height);
		virtual ~CWebAppManager();
		bool Initialize();
		bool RunLoop();
		bool IsRunLoop(){ return m_IsRunLoop; }

#ifndef __EMSCRIPTEN__
		//bool InputState
#endif // !__EMSCRIPTEN__

		// インプットイベント
		void OnKeyDown(std::string key);
		void OnKeyUp(std::string key);

		// リサイズイベント
		void OnResize(int w, int h);

		// マウスイベント
		void OnMouseDown(int buttonNum, int x, int y);
		void OnMouseUp(int buttonNum, int x, int y);
		void OnMouseMove(int x, int y);
		void OnMouseWheel(int deltaY);
	};
}
#endif