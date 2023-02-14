#pragma once
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
		std::shared_ptr<api::CWebGPUAPI> m_pGraphicsAPI;
		std::shared_ptr<app::IApp> m_App;
		
		// インプットプールを作成する
		// 1フレームに1つしか受け取れなかったり、途中で処理を挟まれたりするのもよくないので、1フレームの間のインプットをプールに溜めて
		// まとめて実行する

	private:
		bool Release();
	public:
		CWebAppManager(app::EAppType AppType);
		virtual ~CWebAppManager();
		bool Initialize();
		bool RunLoop();
		bool IsRunLoop(){ return m_IsRunLoop; }

#ifndef __EMSCRIPTEN__
		//bool InputState
#endif // !__EMSCRIPTEN__

		// インプットイベント
		void OnKeyDown(std::string key);
	};
}