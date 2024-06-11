#include "CScriptApp.h"
#include "Scene/CScriptScene.h"

#ifdef USE_VIEWER_CAMERA
#include "../../Camera/CViewerCamera.h"
#else
#include "../../Camera/CCamera.h"
#endif

namespace app
{
	CScriptApp::CScriptApp():
		m_ScriptScene(nullptr),
#ifdef USE_VIEWER_CAMERA
		m_MainCamera(std::make_shared<camera::CViewerCamera>()),
#else
		m_MainCamera(std::make_shared<camera::CCamera>()),
#endif // USE_VIEWER_CAMERA
		m_Projection(std::make_shared<projection::CProjection>()),
		m_DrawInfo(std::make_shared<graphics::CDrawInfo>())
	{
		m_MainCamera->SetPos(glm::vec3(0.0f, 0.0f, 5.0f));
	}

	CScriptApp::~CScriptApp()
	{
	}

	bool CScriptApp::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_ScriptScene)
		{
			m_ScriptScene.reset();
			m_ScriptScene = nullptr;
		}

		return true;
	}

	bool CScriptApp::Initialize(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		m_ScriptScene = std::make_shared<app::CScriptScene>(pGraphicsAPI, pLoadWorker, pPhysicsEngine);

		return true;
	}

	bool CScriptApp::ProcessInput(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CScriptApp::Resize(int Width, int Height)
	{
		m_Projection->SetScreenResolution(Width, Height);

		m_DrawInfo->GetLightProjection()->SetScreenResolution(Width, Height);

		return true;
	}

	bool CScriptApp::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_ScriptScene->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, m_MainCamera, m_Projection, m_DrawInfo, InputState)) return false;

		m_MainCamera->Update(m_DrawInfo->GetDeltaSecondsTime(), InputState);

		return true;
	}

	bool CScriptApp::LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		if (!m_ScriptScene->LateUpdate(pGraphicsAPI, pPhysicsEngine, pLoadWorker, m_DrawInfo)) return false;

		return true;
	}

	bool CScriptApp::FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		if (!m_ScriptScene->FixedUpdate(pGraphicsAPI, pPhysicsEngine, pLoadWorker, m_DrawInfo)) return false;

		return true;
	}

	bool CScriptApp::Draw(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<gui::IGUIEngine>& GUIEngine)
	{
		if (!pGraphicsAPI->BeginRender()) return false;

		if (!m_ScriptScene->Draw(pGraphicsAPI, false, m_MainCamera, m_Projection, m_DrawInfo)) return false;

		if (!pGraphicsAPI->EndRender()) return false;

		return true;
	}

	const std::shared_ptr<graphics::CDrawInfo>& CScriptApp::GetDrawInfo() const
	{
		return m_DrawInfo;
	}

	std::vector<std::shared_ptr<object::C3DObject>> CScriptApp::GetObjectList() const
	{
		return m_ScriptScene->GetObjectList();
	}

	// フォーカスイベント
	void CScriptApp::OnFocus(bool Focused, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
	}
}