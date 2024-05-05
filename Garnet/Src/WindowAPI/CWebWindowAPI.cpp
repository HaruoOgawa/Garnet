#ifdef USE_WEB_NATIVE
#include "CWebWindowAPI.h"
#include "../Message/Console.h"

#include "../GraphicsAPI/WebGPU/CWebGPUAPI.h"

#include "../AppCore/CAppCore.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__

#ifdef USE_VIEWER_CAMERA
#include "../../Camera/CViewerCamera.h"
#endif // USE_VIEWER_CAMERA

#include "../Input/CInputState.h"

#ifdef USE_GUIENGINE
#include "../GUIEngine/imgui/CImGuiGUIEngine.h"
#else
#include "../GUIEngine/CDummyGUIEngine.h"
#endif

namespace webapp
{
	CWebWindowAPI::CWebWindowAPI(int Width, int Height):
		m_IsRunLoop(true),
		m_GraphicsAPI(nullptr),
		m_AppCore(nullptr),
		m_SecondsTime(0.0f),
		m_DeltaSecondsTime(0.0f),
		m_Width(Width),
		m_Height(Height)
	{
		m_GraphicsAPI = std::make_shared<api::CWebGPUAPI>(Width, Height);

		m_AppCore = std::make_shared<app::CAppCore>();
	}

	CWebWindowAPI::~CWebWindowAPI()
	{
		Release();
	}

	bool CWebWindowAPI::Release()
	{
		if (m_AppCore)
		{
			m_AppCore->Release(m_GraphicsAPI.get());
			m_AppCore.reset();
			m_AppCore = nullptr;
		}

		if (m_GraphicsAPI)
		{
			m_GraphicsAPI->Release();
			m_GraphicsAPI.reset();
			m_GraphicsAPI = nullptr;
		}

		return true;
	}

	const std::shared_ptr<app::CAppCore>& CWebWindowAPI::GetAppCore() const
	{
		return m_AppCore;
	}

	bool CWebWindowAPI::Initialize()
	{
		if (!m_GraphicsAPI->Initialize()) return false;

		if (!m_AppCore->Initialize(m_GraphicsAPI.get(), this)) return false;
		
		m_GraphicsAPI->Resize(m_Width, m_Height);
		m_AppCore->Resize(m_Width, m_Height);

		return true;
	}

	bool CWebWindowAPI::RunLoop()
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

			m_AppCore->GetInputState()->Clear();
		}

		return true;
	}

	bool CWebWindowAPI::Update()
	{
		float PrevSecondsTime = m_SecondsTime;
		m_SecondsTime = static_cast<float>(clock()) * 0.001f * 0.001f;
		m_DeltaSecondsTime = m_SecondsTime - PrevSecondsTime;

		if (!m_AppCore->Update(m_SecondsTime, m_DeltaSecondsTime)) return false;

		return true;
	}

	bool CWebWindowAPI::LateUpdate()
	{
		if (!m_AppCore->LateUpdate(m_GraphicsAPI.get())) return false;

		return true;
	}

	bool CWebWindowAPI::FixedUpdate()
	{
		if (!m_AppCore->FixedUpdate(m_GraphicsAPI.get())) return false;

		return true;
	}

	bool CWebWindowAPI::Draw()
	{
		if (!m_AppCore->Draw(m_GraphicsAPI.get())) return false;

		return true;
	}

	// インプットイベント
	void CWebWindowAPI::OnKeyDown(std::string key)
	{
		KeyAction(key, true);
	}
	
	void CWebWindowAPI::OnKeyUp(std::string key)
	{
		KeyAction(key, false);
	}

	void CWebWindowAPI::KeyAction(std::string key, bool IsDown)
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

		auto InputState = m_AppCore->GetInputState();
		InputState->SetKeyState(KeyType, IsDown);

		//
		if (key == "Escape" && IsDown)
		{
			// Webアプリでは止める必要がない
			//m_IsRunLoop = false;
		}
	}

	// リサイズイベント
	void CWebWindowAPI::OnResize(int w, int h)
	{
		m_GraphicsAPI->Resize(w, h);
		m_AppCore->Resize(w, h);
	}

	// マウスイベント
	void CWebWindowAPI::OnMouseDown(int buttonNum, int x, int y)
	{
		auto InputState = m_AppCore->GetInputState();

		if (buttonNum == 0 && !InputState->IsDownMouseRight())
		{
			InputState->SetDownMouseLeft(true);
		}
		else if (buttonNum == 2 && !InputState->IsDownMouseLeft())
		{
			InputState->SetDownMouseRight(true);
		}

		// 位置を正規化する
		float rPosX = static_cast<float>(x) / static_cast<float>(m_Width);
		float rPosY = static_cast<float>(y) / static_cast<float>(m_Height);

		rPosX = rPosX * 2.0f - 1.0f;
		rPosY = rPosY * 2.0f - 1.0f;

		InputState->StartMousePos(glm::vec2(rPosX, rPosY));
	}

	void CWebWindowAPI::OnMouseUp(int buttonNum, int x, int y)
	{
		auto InputState = m_AppCore->GetInputState();

		if (buttonNum == 0 && !InputState->IsDownMouseRight())
		{
			InputState->SetDownMouseLeft(false);
		}
		else if (buttonNum == 2 && !InputState->IsDownMouseLeft())
		{
			InputState->SetDownMouseRight(false);
		}

		// 位置を正規化する
		float rPosX = static_cast<float>(x) / static_cast<float>(m_Width);
		float rPosY = static_cast<float>(y) / static_cast<float>(m_Height);

		rPosX = rPosX * 2.0f - 1.0f;
		rPosY = rPosY * 2.0f - 1.0f;

		InputState->StartMousePos(glm::vec2(rPosX, rPosY));
	}

	void CWebWindowAPI::OnMouseMove(int x, int y)
	{
		auto InputState = m_AppCore->GetInputState();

		if (InputState->IsDownMouseLeft() || InputState->IsDownMouseRight())
		{
			// 位置を正規化する
			float rPosX = static_cast<float>(x) / static_cast<float>(m_Width);
			float rPosY = static_cast<float>(y) / static_cast<float>(m_Height);

			rPosX = rPosX * 2.0f - 1.0f;
			rPosY = rPosY * 2.0f - 1.0f;

			InputState->SetMousePos(glm::vec2(rPosX, rPosY));
		}
	}

	void CWebWindowAPI::OnMouseWheel(int deltaY)
	{
		auto InputState = m_AppCore->GetInputState();

		// ブラウザだとピクセルに基づくホイール量が -150 ~ 150の範囲で返ってくるのでひとまず -1.0 ~ 1.0fにしておく
		float wheelRate = glm::sign(-1.0f * static_cast<float>(deltaY)) * 1.0f;
		InputState->SetWheelScrollAmount(glm::vec2(0.0f, wheelRate));
	}
}

#endif // USE_WEB_NATIVE