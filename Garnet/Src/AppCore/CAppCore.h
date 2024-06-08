#pragma once

#include "SAppSettings.h"
#include "../Interface/IWindowAPI.h"
#include "../Interface/IApp.h"
#include <string>
#include <memory>

namespace api { class IGraphicsAPI; }
namespace app { class IApp; }
namespace input { class CInputState; }
namespace resource { class CLoadWorker; }
namespace gui { class IGUIEngine; }
namespace window { class IWindowAPI; }
namespace physics { class IPhysicsEngine; }

namespace app
{
	class CAppCore
	{
		SAppSettings m_AppSettings;

		std::shared_ptr<window::IWindowAPI> m_WindowAPI;

		std::shared_ptr<api::IGraphicsAPI> m_GraphicsAPI;

		bool m_IsRunLoop;

		float m_SecondsTime;
		float m_DeltaSecondsTime;

		std::shared_ptr<app::IApp> m_App;

		std::shared_ptr<resource::CLoadWorker> m_LoadWorker;

		std::shared_ptr<physics::IPhysicsEngine> m_PhysicsEngine;

		std::shared_ptr<gui::IGUIEngine> m_GUIEngine;

		std::shared_ptr<input::CInputState> m_InputState;
	public:
		CAppCore(const std::shared_ptr<app::IApp>& App, SAppSettings Settings);
		virtual ~CAppCore();

		const std::shared_ptr<gui::IGUIEngine>& GetGUIEngine() const;

		const std::shared_ptr<input::CInputState>& GetInputState()const { return m_InputState; }

		bool Initialize();

		bool Release();
		
		bool ResizeWindow(int Width, int Height);
		void FocusWindow(bool Focused);
		
		bool RunLoop();

		void SetRunLoop(bool Flag);
		bool IsRunLoop();
		
		bool Update();
		bool LateUpdate();
		bool FixedUpdate();
		bool Draw();

		// ToDo: 将来的にはOnXX関数を各WindowAPIから呼ぶようにしてResizeWindow・FocusWindowを統一する. InputStateへの情報受け渡しもここでやる
		// インプットイベント
		void OnKeyDown(std::string key);
		void OnKeyUp(std::string key);

		// リサイズイベント
		void OnResize(int w, int h);

		// フォーカスイベント
		void OnFocus(int focused);

		// マウスイベント
		void OnMouseDown(int buttonNum, int x, int y);
		void OnMouseUp(int buttonNum, int x, int y);
		void OnMouseMove(int x, int y);
		void OnMouseWheel(int deltaY);
	};
}