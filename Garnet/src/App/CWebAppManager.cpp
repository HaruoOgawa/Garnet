#ifdef __EMSCRIPTEN__
#include "CWebAppManager.h"
#include "../Debug/Message/Console.h"

#include "../GraphicsAPI/WebGPU/CWebGPUAPI.h"
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
		m_GraphicsAPI(nullptr),
		m_App(nullptr),
		m_SecondsTime(0.0f),
		m_DeltaSecondsTime(0.0f)
	{
		Console::Log("CWebAppManager::CWebAppManager\n");

		//
		m_GraphicsAPI = std::make_shared<api::CWebGPUAPI>();

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

		if (m_App)
		{
			m_App->Release(m_GraphicsAPI.get());
			m_App.reset();
			m_App = nullptr;
		}

		if (m_GraphicsAPI)
		{
			m_GraphicsAPI->Release();
			m_GraphicsAPI.reset();
			m_GraphicsAPI = nullptr;
		}

		return true;
	}

	bool CWebAppManager::Initialize()
	{
		Console::Log("CWebAppManager::Initialize\n");

		if (!m_GraphicsAPI->Initialize()) return false;

		if (!m_App->Initialize(m_GraphicsAPI.get())) return false;
		
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
			if (!Update()) return false;
			if (!Draw()) return false;
		}

		return true;
	}

	bool CWebAppManager::Update()
	{
		float PrevSecondsTime = m_SecondsTime;
		m_SecondsTime = static_cast<float>(clock()) * 0.001f * 0.001f;
		m_DeltaSecondsTime = m_SecondsTime - PrevSecondsTime;

		if (!m_App->Update(m_GraphicsAPI.get(), m_SecondsTime)) return false;

#ifdef _DEBUG
		// FPSの計測と表示(60FPSを基準とする)
		float FPS = 60.0f / (m_DeltaSecondsTime * 60.0f);
		Console::Log("[FPS] %f fps / [CurrentTime] %f s\n", FPS, m_SecondsTime);
#endif // _DEBUG

		if (!m_App->Update(m_GraphicsAPI.get(), m_SecondsTime)) return false;

		return true;
	}

	bool CWebAppManager::Draw()
	{
		if (!m_App->Draw(m_GraphicsAPI.get())) return false;

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

#endif