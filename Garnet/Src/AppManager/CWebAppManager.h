#pragma once
#ifdef USE_WEB_NATIVE
#include <string>
#include <memory>
#include "../Interface/IWindowAPI.h"
#include "EAppType.h"

namespace api { class CWebGPUAPI; }
namespace app { class CAppCore; }
namespace input { class CInputState; }

namespace webapp
{
	class CWebAppManager : public IWindowAPI
	{
		bool m_IsRunLoop;
		std::shared_ptr<api::CWebGPUAPI> m_GraphicsAPI;
		std::shared_ptr<app::CAppCore> m_AppCore;
		
		std::shared_ptr<input::CInputState> m_InputState;

		float m_SecondsTime;
		float m_DeltaSecondsTime;

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

		const std::shared_ptr<app::CAppCore>& GetAppCore() const;

		bool Initialize();
		bool RunLoop();
		bool IsRunLoop(){ return m_IsRunLoop; }

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
#endif // USE_WEB_NATIVE