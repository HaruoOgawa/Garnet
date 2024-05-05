#pragma once

#include "../Interface/IWindowAPI.h"
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
		CAppCore(const std::shared_ptr<app::IApp>& App);
		virtual ~CAppCore();

		const std::shared_ptr<gui::IGUIEngine>& GetGUIEngine() const;

		const std::shared_ptr<input::CInputState>& GetInputState()const { return m_InputState; }

		bool Initialize(int Width, int Height);

		bool Release();
		
		bool Resize(int Width, int Height);
		
		bool RunLoop();

		void SetRunLoop(bool Flag);
		bool IsRunLoop();
		
		bool Update();
		bool LateUpdate();
		bool FixedUpdate();
		bool Draw();

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