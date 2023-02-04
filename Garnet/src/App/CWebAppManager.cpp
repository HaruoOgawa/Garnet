#include "CWebAppManager.h"
#include "../Debug/Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__


namespace webapp
{
	CWebAppManager::CWebAppManager():
		m_RunLoop(true)
	{
		Console::Log("CWebAppManager::CWebAppManager\n");
	}

	CWebAppManager::~CWebAppManager()
	{
		Release();

		Console::Log("CWebAppManager::~CWebAppManager\n");
	}

	bool CWebAppManager::Release()
	{
		Console::Log("CWebAppManager::Release\n");
		return true;
	}

	bool CWebAppManager::Initialize()
	{
		Console::Log("CWebAppManager::Initialize\n");
		return true;
	}

	bool CWebAppManager::RunLopp()
	{
		//Console::Log("CWebAppManager::RunLopp\n");

		if (!m_RunLoop)
		{
#ifdef __EMSCRIPTEN__
			emscripten_cancel_main_loop();
#endif // __EMSCRIPTEN__
		}

		return true;
	}

	// インプットイベント
	void CWebAppManager::OnKeyDown(std::string key)
	{
		Console::Log("CWebAppManager::OnkeyDown / Key: %s\n", key.c_str());

		if (key == "Escape")
		{
			m_RunLoop = false;
		}
	}
}