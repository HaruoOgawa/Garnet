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

#ifdef USE_VIEWER_CAMERA
#include "../../Camera/CViewerCamera.h"
#endif // USE_VIEWER_CAMERA

#include "../Input/CInputState.h"

namespace webapp
{
	CWebAppManager::CWebAppManager(app::EAppType AppType, int Width, int Height):
		m_IsRunLoop(true),
		m_GraphicsAPI(nullptr),
		m_App(nullptr),
		m_InputState(std::make_shared<input::CInputState>(1.0f)),
		m_SecondsTime(0.0f),
		m_DeltaSecondsTime(0.0f),
		m_Width(Width),
		m_Height(Height)
	{
		//
		m_GraphicsAPI = std::make_shared<api::CWebGPUAPI>(Width, Height);

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
	}

	bool CWebAppManager::Release()
	{
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
		if (!m_GraphicsAPI->Initialize()) return false;

		if (!m_App->Initialize(m_GraphicsAPI.get())) return false;
		
		m_GraphicsAPI->Resize(m_Width, m_Height);
		m_App->Resize(m_Width, m_Height);

		return true;
	}

	bool CWebAppManager::RunLoop()
	{
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

			m_InputState->Clear();
		}

		return true;
	}

	bool CWebAppManager::Update()
	{
		float PrevSecondsTime = m_SecondsTime;
		m_SecondsTime = static_cast<float>(clock()) * 0.001f * 0.001f;
		m_DeltaSecondsTime = m_SecondsTime - PrevSecondsTime;

		// ViewCameraのUpdate
#ifdef USE_INPUT_SYSTEM
		const auto& MainCamera = m_App->GetMainCamera();
		if (MainCamera) MainCamera->Update(m_SecondsTime, m_InputState);
#endif // USE_INPUT_SYSTEM

		//
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
		if (!m_App->Draw(m_GraphicsAPI.get(), m_SecondsTime)) return false;

		return true;
	}

	// インプットイベント
	void CWebAppManager::OnKeyDown(std::string key)
	{
		if (key == "Escape")
		{
			m_IsRunLoop = false;
		}
	}

	// リサイズイベント
	void CWebAppManager::OnResize(int w, int h)
	{
		m_GraphicsAPI->Resize(w, h);
		m_App->Resize(w, h);
	}

	// マウスイベント
	void CWebAppManager::OnMouseDown(int buttonNum, int x, int y)
	{
		if (buttonNum == 0)
		{
			m_InputState->SetDownMouseLeft(true);

			// 位置を正規化する
			float rPosX = static_cast<float>(x) / static_cast<float>(m_Width);
			float rPosY = static_cast<float>(y) / static_cast<float>(m_Height);

			rPosX = rPosX * 2.0f - 1.0f;
			rPosY = rPosY * 2.0f - 1.0f;

			m_InputState->StartMousePos(glm::vec2(rPosX, rPosY));
		}
	}

	void CWebAppManager::OnMouseUp(int buttonNum, int x, int y)
	{
		if (buttonNum == 0)
		{
			m_InputState->SetDownMouseLeft(false);

			// 位置を正規化する
			float rPosX = static_cast<float>(x) / static_cast<float>(m_Width);
			float rPosY = static_cast<float>(y) / static_cast<float>(m_Height);

			rPosX = rPosX * 2.0f - 1.0f;
			rPosY = rPosY * 2.0f - 1.0f;

			m_InputState->StartMousePos(glm::vec2(rPosX, rPosY));
		}
	}

	void CWebAppManager::OnMouseMove(int x, int y)
	{
		if (m_InputState->IsDownMouseLeft())
		{
			// 位置を正規化する
			float rPosX = static_cast<float>(x) / static_cast<float>(m_Width);
			float rPosY = static_cast<float>(y) / static_cast<float>(m_Height);

			rPosX = rPosX * 2.0f - 1.0f;
			rPosY = rPosY * 2.0f - 1.0f;

			m_InputState->SetMousePos(glm::vec2(rPosX, rPosY));
		}
	}
}

#endif