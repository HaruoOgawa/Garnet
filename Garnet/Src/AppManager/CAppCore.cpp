#include "CAppCore.h"
#include "../Interface/IWindowAPI.h"
#include "../App/ScriptApp/CScriptApp.h"
#include "../LoadWorker/CLoadWorker.h"
#include "../Input/CInputState.h"
#include "../Camera/CCamera.h"
#include "../Graphics/CDrawInfo.h"

#ifdef USE_GUIENGINE
#include "../GUIEngine/imgui/CImGuiGUIEngine.h"
#else
#include "../GUIEngine/CDummyGUIEngine.h"
#endif

namespace app
{
	CAppCore::CAppCore():
		m_App(nullptr),
		m_LoadWorker(nullptr),
		m_GUIEngine(nullptr)
	{
		m_App = std::make_shared<app::CScriptApp>();

#ifdef USE_GUIENGINE
		m_GUIEngine = std::make_shared<gui::CImGuiGUIEngine>();
#else
		m_GUIEngine = std::make_shared<gui::CDummyGUIEngine>();
#endif
	}

	bool CAppCore::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_App)
		{
			m_App->Release(pGraphicsAPI);
			m_App.reset();
			m_App = nullptr;
		}

		if (m_LoadWorker)
		{
			m_LoadWorker.reset();
			m_LoadWorker = nullptr;
		}

		if (m_GUIEngine)
		{
			m_GUIEngine->Release(pGraphicsAPI);
			m_GUIEngine.reset();
			m_GUIEngine = nullptr;
		}

		return true;
	}

	const std::shared_ptr<gui::IGUIEngine>& CAppCore::GetGUIEngine() const
	{
		return m_GUIEngine;
	}

	bool CAppCore::Initialize(api::IGraphicsAPI* pGraphicsAPI, IWindowAPI* pWindowAPI)
	{
		// ロードワーカー
		m_LoadWorker = std::make_shared<resource::CLoadWorker>(pGraphicsAPI);

#ifdef USE_GUIENGINE
#ifdef USE_GLFW
		if (!m_GUIEngine->InitializeWithGLFW(pWindowAPI->GetGLFWWindow(), pGraphicsAPI)) return false;
#elif USE_WIN32_WindowAPI
		if (!m_GUIEngine->InitializeWithWin32API(pWindowAPI->GetWin32Window(), pGraphicsAPI)) return false;
#endif
#endif // USE_GUIENGINE

		//
		if (!m_App->Initialize(pGraphicsAPI, m_LoadWorker.get())) return false;

		return true;
	}

	bool CAppCore::Resize(int Width, int Height)
	{
		m_App->Resize(Width, Height);

		return true;
	}

	bool CAppCore::Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<input::CInputState>& InputState, float SecondsTime, float DeltaSecondsTime)
	{
		m_App->GetDrawInfo()->SetSecondsTime(SecondsTime);
		m_App->GetDrawInfo()->SetDeltaSecondsTime(DeltaSecondsTime);

		const auto& MainCamera = m_App->GetMainCamera();
		if (MainCamera) MainCamera->Update(DeltaSecondsTime, InputState);

		if (!m_App->Update(pGraphicsAPI, m_LoadWorker.get(), InputState)) return false;

		return true;
	}

	bool CAppCore::LateUpdate(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_App->LateUpdate(pGraphicsAPI, m_LoadWorker.get())) return false;

		return true;
	}

	bool CAppCore::FixedUpdate(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_App->FixedUpdate(pGraphicsAPI, m_LoadWorker.get())) return false;

		return true;
	}

	bool CAppCore::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_App->Draw(pGraphicsAPI, m_LoadWorker.get(), m_GUIEngine)) return false;

		return true;
	}
}