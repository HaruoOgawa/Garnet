#include "CAppCore.h"
#include "../App/ScriptApp/CScriptApp.h"
#include "../Input/CInputState.h"
#include "../Camera/CCamera.h"
#include "../Graphics/CDrawInfo.h"

namespace app
{
	CAppCore::CAppCore():
		m_App(nullptr)
	{
		m_App = std::make_shared<app::CScriptApp>();
	}

	CAppCore::~CAppCore()
	{
	}

	bool CAppCore::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		if (!m_App->Initialize(pGraphicsAPI, pLoadWorker)) return false;

		return true;
	}

	bool CAppCore::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_App)
		{
			m_App->Release(pGraphicsAPI);
			m_App.reset();
			m_App = nullptr;
		}

		return true;
	}

	bool CAppCore::Resize(int Width, int Height)
	{
		m_App->Resize(Width, Height);

		return true;
	}

	bool CAppCore::Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<input::CInputState>& InputState, float SecondsTime, float DeltaSecondsTime)
	{
		m_App->GetDrawInfo()->SetSecondsTime(SecondsTime);
		m_App->GetDrawInfo()->SetDeltaSecondsTime(DeltaSecondsTime);

		const auto& MainCamera = m_App->GetMainCamera();
		if (MainCamera) MainCamera->Update(DeltaSecondsTime, InputState);

		if (!m_App->Update(pGraphicsAPI, pLoadWorker, InputState)) return false;

		return true;
	}

	bool CAppCore::LateUpdate(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		if (!m_App->LateUpdate(pGraphicsAPI, pLoadWorker)) return false;

		return true;
	}

	bool CAppCore::FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		if (!m_App->FixedUpdate(pGraphicsAPI, pLoadWorker)) return false;

		return true;
	}

	bool CAppCore::Draw(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<gui::IGUIEngine>& GUIEngine)
	{
		if (!m_App->Draw(pGraphicsAPI, pLoadWorker, GUIEngine)) return false;

		return true;
	}
}