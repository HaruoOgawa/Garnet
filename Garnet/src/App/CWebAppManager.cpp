#include "CWebAppManager.h"
#include "../Debug/Message/Console.h"

#include "../GraphicsAPI/CWebGPUAPI.h"
#include "./ScriptApp/CScriptApp.h"
#include "./EditorApp/CEditorApp.h"
#include "./MainApp/CMainApp.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__

namespace webapp
{
	CWebAppManager::CWebAppManager(app::EAppType AppType):
		m_IsRunLoop(true),
		m_pGraphicsAPI(nullptr),
		m_App(nullptr)
	{
		Console::Log("CWebAppManager::CWebAppManager\n");

		//
		m_pGraphicsAPI = std::make_shared<api::CWebGPUAPI>();

		//
		if (AppType == app::EAppType::ScriptApp)
		{
			m_App = std::make_shared<app::CScriptApp>();
		}
		else if (AppType == app::EAppType::EditorApp)
		{
			m_App = std::make_shared<app::CEditorApp>();
		}
		else if (AppType == app::EAppType::MainApp)
		{
			m_App = std::make_shared<app::CMainApp>();
		}
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

	bool CWebAppManager::RunLoop()
	{
		//Console::Log("CWebAppManager::RunLopp\n");

		if (!m_IsRunLoop)
		{
#ifdef __EMSCRIPTEN__
			emscripten_cancel_main_loop();
#endif // __EMSCRIPTEN__
		}
		else
		{

		}

		return true;
	}

	// インプットイベント
	void CWebAppManager::OnKeyDown(std::string key)
	{
		Console::Log("CWebAppManager::OnkeyDown / Key: %s\n", key.c_str());

		if (key == "Escape")
		{
			m_IsRunLoop = false;
		}
	}
}