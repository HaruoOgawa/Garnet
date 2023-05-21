#pragma once
#ifdef __EMSCRIPTEN__
#include <string>
#include <memory>
#include "EAppType.h"

namespace api { class CWebGPUAPI; }
namespace app { class IApp; }

namespace webapp
{
	class CWebAppManager
	{
		bool m_IsRunLoop;
		std::shared_ptr<api::CWebGPUAPI> m_GraphicsAPI;
		std::shared_ptr<app::IApp> m_App;
		
		float m_SecondsTime;
		float m_DeltaSecondsTime;

		int m_Width;
		int m_Height;

	private:
		bool Release();

		bool Update();
		bool Draw();
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

		// リサイズイベント
		void OnResize(int w, int h);
	};
}
#endif