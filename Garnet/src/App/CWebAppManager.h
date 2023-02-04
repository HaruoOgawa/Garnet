#pragma once
#include <string>

namespace webapp
{
	class CWebAppManager
	{
		bool m_RunLoop;

		// インプットプールを作成する
		// 1フレームに1つしか受け取れなかったり、途中で処理を挟まれたりするのもよくないので、1フレームの間のインプットをプールに溜めて
		// まとめて実行する

	private:
		bool Release();
	public:
		CWebAppManager();
		virtual ~CWebAppManager();
		bool Initialize();
		bool RunLopp();
		bool IsRunLoop(){ return m_RunLoop; }

#ifndef __EMSCRIPTEN__
		//bool InputState
#endif // !__EMSCRIPTEN__

		// インプットイベント
		void OnKeyDown(std::string key);
	};
}