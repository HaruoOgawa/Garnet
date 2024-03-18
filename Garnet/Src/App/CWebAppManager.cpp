#ifdef __EMSCRIPTEN__
#include "CWebAppManager.h"
#include "../LoadWorker/CLoadWorker.h"
#include "../Debug/Message/Console.h"

#include "../GraphicsAPI/WebGPU/CWebGPUAPI.h"
#include "./ScriptApp/CScriptApp.h"

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
		m_LoadWorker(nullptr),
		m_Width(Width),
		m_Height(Height)
	{
		m_GraphicsAPI = std::make_shared<api::CWebGPUAPI>(Width, Height);

		m_App = std::make_shared<app::CScriptApp>();
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

		if (m_LoadWorker)
		{
			m_LoadWorker.reset();
			m_LoadWorker = nullptr;
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

		// ロードワーカー
		m_LoadWorker = std::make_shared<resource::CLoadWorker>(m_GraphicsAPI.get());

		if (!m_App->Initialize(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;
		
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
			if (!LateUpdate()) return false;
			if (!FixedUpdate()) return false;
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

		m_App->GetDrawInfo()->SetSecondsTime(m_SecondsTime);
		m_App->GetDrawInfo()->SetDeltaSecondsTime(m_DeltaSecondsTime);

		// ViewCameraのUpdate
#ifdef USE_INPUT_SYSTEM
		const auto& MainCamera = m_App->GetMainCamera();
		if (MainCamera) MainCamera->Update(m_DeltaSecondsTime, m_InputState);
#endif // USE_INPUT_SYSTEM

		if (!m_App->Update(m_GraphicsAPI.get(), m_LoadWorker.get(), m_InputState)) return false;

		return true;
	}

	bool CWebAppManager::LateUpdate()
	{
		if (!m_App->LateUpdate(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

		return true;
	}

	bool CWebAppManager::FixedUpdate()
	{
		if (!m_App->FixedUpdate(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

		return true;
	}

	bool CWebAppManager::Draw()
	{
		if (!m_App->Draw(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

		return true;
	}

	// インプットイベント
	void CWebAppManager::OnKeyDown(std::string key)
	{
		KeyAction(key, true);
	}
	
	void CWebAppManager::OnKeyUp(std::string key)
	{
		KeyAction(key, false);
	}

	void CWebAppManager::KeyAction(std::string key, bool IsDown)
	{
		//
		input::EKeyType KeyType = input::EKeyType::KEY_TYPE_NONE;

		if (key == "w")
		{
			KeyType = input::EKeyType::KEY_TYPE_W;
		}
		else if (key == "a")
		{
			KeyType = input::EKeyType::KEY_TYPE_A;
		}
		else if (key == "s")
		{
			KeyType = input::EKeyType::KEY_TYPE_S;
		}
		else if (key == "d")
		{
			KeyType = input::EKeyType::KEY_TYPE_D;
		}
		else if (key == "1")
		{
			KeyType = input::EKeyType::KEY_TYPE_1;
		}
		else if (key == "2")
		{
			KeyType = input::EKeyType::KEY_TYPE_2;
		}
		else if (key == "3")
		{
			KeyType = input::EKeyType::KEY_TYPE_3;
		}
		else if (key == "4")
		{
			KeyType = input::EKeyType::KEY_TYPE_4;
		}
		else if (key == "5")
		{
			KeyType = input::EKeyType::KEY_TYPE_5;
		}

		m_InputState->SetKeyState(KeyType, IsDown);

		//
		if (key == "Escape" && IsDown)
		{
			// Webアプリでは止める必要がない
			//m_IsRunLoop = false;
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
		if (buttonNum == 0 && !m_InputState->IsDownMouseRight())
		{
			m_InputState->SetDownMouseLeft(true);
		}
		else if (buttonNum == 2 && !m_InputState->IsDownMouseLeft())
		{
			m_InputState->SetDownMouseRight(true);
		}

		// 位置を正規化する
		float rPosX = static_cast<float>(x) / static_cast<float>(m_Width);
		float rPosY = static_cast<float>(y) / static_cast<float>(m_Height);

		rPosX = rPosX * 2.0f - 1.0f;
		rPosY = rPosY * 2.0f - 1.0f;

		m_InputState->StartMousePos(glm::vec2(rPosX, rPosY));
	}

	void CWebAppManager::OnMouseUp(int buttonNum, int x, int y)
	{
		if (buttonNum == 0 && !m_InputState->IsDownMouseRight())
		{
			m_InputState->SetDownMouseLeft(false);
		}
		else if (buttonNum == 2 && !m_InputState->IsDownMouseLeft())
		{
			m_InputState->SetDownMouseRight(false);
		}

		// 位置を正規化する
		float rPosX = static_cast<float>(x) / static_cast<float>(m_Width);
		float rPosY = static_cast<float>(y) / static_cast<float>(m_Height);

		rPosX = rPosX * 2.0f - 1.0f;
		rPosY = rPosY * 2.0f - 1.0f;

		m_InputState->StartMousePos(glm::vec2(rPosX, rPosY));
	}

	void CWebAppManager::OnMouseMove(int x, int y)
	{
		if (m_InputState->IsDownMouseLeft() || m_InputState->IsDownMouseRight())
		{
			// 位置を正規化する
			float rPosX = static_cast<float>(x) / static_cast<float>(m_Width);
			float rPosY = static_cast<float>(y) / static_cast<float>(m_Height);

			rPosX = rPosX * 2.0f - 1.0f;
			rPosY = rPosY * 2.0f - 1.0f;

			m_InputState->SetMousePos(glm::vec2(rPosX, rPosY));
		}
	}

	void CWebAppManager::OnMouseWheel(int deltaY)
	{
		// ブラウザだとピクセルに基づくホイール量が -150 ~ 150の範囲で返ってくるのでひとまず -1.0 ~ 1.0fにしておく
		float wheelRate = glm::sign(-1.0f * static_cast<float>(deltaY)) * 1.0f;
		m_InputState->SetWheelScrollAmount(glm::vec2(0.0f, wheelRate));
	}
}

#endif