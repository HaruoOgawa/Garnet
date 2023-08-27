#include "CScriptApp.h"
#include "../../LoadWorker/CLoadWorker.h"
#include "../../Scene/CScriptScene.h"
#include "../../Graphics/CDrawInfo.h"
#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"
#include "../../ImageEffect/CBlurEffect.h"

#ifdef USE_VIEWER_CAMERA
#include "../../Camera/CViewerCamera.h"
#endif // USE_VIEWER_CAMERA

// CScriptApp は旧エンジンでもやっていたof風にCppでエンジンコードを直接シーンを構築していくアプリ

// MVVMで設計する
// App => ViewModel, Scene => View, API => Model
// Garnetで作品を作る時、View(Scene)とViewModel(App)は編集していいが、Modelの変更は一切許さない

// FrameBufferListはAppで作り、そのテクスチャリストをViewにInitializeの最後辺りで渡す

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
		m_DrawInfo->GetLightCamera()->SetPos(glm::vec3(3.0f, 3.0f, -3.0f));
		m_DrawInfo->GetLightProjection()->SetNear(2.0f);
		m_DrawInfo->GetLightProjection()->SetFar(100.0f);
	}

	CScriptApp::~CScriptApp()
	{
	}

	bool CScriptApp::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CScriptApp::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		// Viewの初期化
		m_ScriptScene = std::make_shared<scene::CScriptScene>(pGraphicsAPI, pLoadWorker);

		return true;
	}

	bool CScriptApp::ProcessInput(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CScriptApp::Resize(int Width, int Height)
	{
		m_Projection->SetAspect(static_cast<float>(Width) / static_cast<float>(Height));
		m_DrawInfo->GetLightProjection()->SetAspect(static_cast<float>(Width) / static_cast<float>(Height));

		return true;
	}

	bool CScriptApp::Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, float SecondsTime)
	{
		if (!pLoadWorker->Update(pGraphicsAPI)) return false;

		if (!m_ScriptScene->Update(pGraphicsAPI, pLoadWorker, SecondsTime, m_MainCamera, m_Projection, m_DrawInfo)) return false;

		return true;
	}

	bool CScriptApp::Draw(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, float SecondsTime)
	{
		// Prepare
		if (!pGraphicsAPI->PrepareRender()) return false;

		// Dispatch GPGPU
		if (!m_ScriptScene->Dispatch(pGraphicsAPI, pLoadWorker, SecondsTime, m_MainCamera, m_Projection, m_DrawInfo)) return false;

		// DefaultPass(SwapChain)
		if (!pGraphicsAPI->BeginRender()) return false;

		if (!m_ScriptScene->Draw(pGraphicsAPI, false, SecondsTime, m_MainCamera, m_Projection, m_DrawInfo)) return false;
		if (!pLoadWorker->Draw(pGraphicsAPI, false, SecondsTime, m_MainCamera, m_Projection, m_DrawInfo)) return false;
		
		if (!pGraphicsAPI->EndRender()) return false;

		// Submit
		if (!pGraphicsAPI->SubmitRender()) return false;

		return true;
	}

	const std::shared_ptr<camera::CCamera>& CScriptApp::GetMainCamera() const
	{
		return m_MainCamera;
	}
}