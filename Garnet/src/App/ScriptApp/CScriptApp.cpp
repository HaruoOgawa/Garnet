#include "CScriptApp.h"
#include "../../Scene/CScriptScene.h"
#include "../../GraphicsAPI/ERenderPassType.h"

namespace app
{
	CScriptApp::CScriptApp():
		m_ScriptScene(nullptr)
	{
	}

	CScriptApp::~CScriptApp()
	{
	}

	bool CScriptApp::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_ScriptScene)
		{
			m_ScriptScene->Release(pGraphicsAPI);
		}

		return true;
	}

	bool CScriptApp::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		m_ScriptScene = std::make_shared<scene::CScriptScene>();
		if (!m_ScriptScene->Initialize(pGraphicsAPI)) return false;

		return true;
	}

	bool CScriptApp::ProcessInput(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CScriptApp::Update(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime)
	{
		if (!m_ScriptScene->Update(pGraphicsAPI, SecondsTime)) return false;

		return true;
	}

	bool CScriptApp::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!pGraphicsAPI->BeginRender(api::ERenderPassType::FORWARD_POLYGONE)) return false;

		if (!m_ScriptScene->Draw(pGraphicsAPI)) return false;

		if (!pGraphicsAPI->EndRender()) return false;

		return true;
	}
}