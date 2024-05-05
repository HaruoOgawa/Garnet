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

namespace window
{
	CWebWindowAPI::CWebWindowAPI():
		m_pCAppCore(nullptr),
		m_Width(1920),
		m_Height(1080)
	{
	}

	bool CWebWindowAPI::Release()
	{
		return true;
	}

	bool CWebWindowAPI::Initialize(app::CAppCore* pAppCore, int Width, int Height)
	{
		m_pCAppCore = pAppCore;

		m_Width = Width;
		m_Height = Height;

		return true;
	}
	
	void CWebWindowAPI::SwapWindowBuffers()
	{
	}

	void CWebWindowAPI::AssignCurrentWindowSize()
	{
	}

	void CWebWindowAPI::PollEvents()
	{
	}

	app::CAppCore* CWebWindowAPI::GetAppCore() const
	{
		return m_pCAppCore;
	}

	void CWebWindowAPI::ResizeWindow(int w, int h)
	{
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

		auto InputState = m_pCAppCore->GetInputState();
		InputState->SetKeyState(KeyType, IsDown);

		//
		if (key == "Escape" && IsDown)
		{
			// Webアプリでは止める必要がない
			// m_pCAppCore->SetRunLoop(false);
		}
	}

	// リサイズイベント
	void CWebWindowAPI::OnResize(int w, int h)
	{
		m_pCAppCore->Resize(w, h);
	}

	// マウスイベント
	void CWebWindowAPI::OnMouseDown(int buttonNum, int x, int y)
	{
		auto InputState = m_pCAppCore->GetInputState();

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
		auto InputState = m_pCAppCore->GetInputState();

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
		auto InputState = m_pCAppCore->GetInputState();

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
		auto InputState = m_pCAppCore->GetInputState();

		// ブラウザだとピクセルに基づくホイール量が -150 ~ 150の範囲で返ってくるのでひとまず -1.0 ~ 1.0fにしておく
		float wheelRate = glm::sign(-1.0f * static_cast<float>(deltaY)) * 1.0f;
		InputState->SetWheelScrollAmount(glm::vec2(0.0f, wheelRate));
	}
}

#endif // USE_WEB_NATIVE