#include "CScriptApp.h"
#include "../../LoadWorker/CLoadWorker.h"
#include "Scene/CScriptScene.h"
#include "../../Graphics/CDrawInfo.h"
#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"
#include "../../ImageEffect/CBlurEffect.h"
#include "../../Message/Console.h"
#include "../../Interface/IGUIEngine.h"
#ifdef USE_VIEWER_CAMERA
#include "../../Camera/CViewerCamera.h"
#endif // USE_VIEWER_CAMERA

#include "GUI/CGraphicsEditingWindow.h"
#include "Model/CFileModifier.h"

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
		m_DrawInfo(std::make_shared<graphics::CDrawInfo>()),
#ifdef USE_GUIENGINE
		m_GraphicsEditingWindow(std::make_shared<gui::CGraphicsEditingWindow>()),
#endif // USE_GUIENGINE
		m_BlurEffect(nullptr),
		m_FileModifier(std::make_shared<CFileModifier>())
	{
		m_MainCamera->SetPos(glm::vec3(0.0f, 1.0f, -7.0f));
		//m_MainCamera->SetCenter(glm::vec3(0.0f, 50.0f, 349.0f));
		//m_MainCamera->SetPos(glm::vec3(0.0f, 50.0f, 350.0f));
		m_DrawInfo->GetLightCamera()->SetPos(glm::vec3(-2.358f, 15.6f, -0.59f));
		m_DrawInfo->GetLightProjection()->SetNear(2.0f);
		m_DrawInfo->GetLightProjection()->SetFar(100.0f);
	}

	bool CScriptApp::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_ScriptScene)
		{
			m_ScriptScene.reset();
			m_ScriptScene = nullptr;
		}
		
		if (m_BlurEffect)
		{
			m_BlurEffect.reset();
			m_BlurEffect = nullptr;
		}
		
		return true;
	}

	bool CScriptApp::Initialize(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		// Viewの初期化
		m_ScriptScene = std::make_shared<app::CScriptScene>(pGraphicsAPI, pLoadWorker, pPhysicsEngine);

		// オフスクリーンレンダリング
		//if (!pGraphicsAPI->CreateRenderPass("ShadowPass", api::ERenderPassFormat::COLOR_RENDERPASS, glm::vec4(1.0f), 512, 512)) return false;
		if (!pGraphicsAPI->CreateRenderPass("MRTTest", api::ERenderPassFormat::COLOR_RENDERPASS, glm::vec4(1.0f), 512, 512, 3)) return false;

		m_BlurEffect = std::make_shared<imageeffect::CBlurEffect>(pGraphicsAPI);
		if (!m_BlurEffect->Create(pLoadWorker)) return false;

		// FrameTextureを渡す
		//m_ScriptScene->SetFrameTexture(m_BlurEffect->GetFrameTexture());

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
		if (!m_FileModifier->Update(pLoadWorker)) return false;

		if (!m_ScriptScene->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, m_MainCamera, m_Projection, m_DrawInfo, InputState)) return false;

		if (!m_BlurEffect->Update(pLoadWorker)) return false;

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
		/*// Dispatch GPGPU
		if (!m_ScriptScene->Dispatch(pGraphicsAPI, pLoadWorker, m_MainCamera, m_Projection, m_DrawInfo)) return false;

		// ShadowPass
		if (!pGraphicsAPI->BeginRender("ShadowPass")) return false;
		if (!m_ScriptScene->Draw(pGraphicsAPI, true, m_MainCamera, m_Projection, m_DrawInfo)) return false;
		if (!pGraphicsAPI->EndRender()) return false;

		// ShadowMapにブラーをかける
		if (!m_BlurEffect->Draw(m_MainCamera, m_Projection, m_DrawInfo)) return false;*/
		
		// MRTTest FrameBuffer
		{
			if (!pGraphicsAPI->BeginRender("MRTTest")) return false;
			if (!m_ScriptScene->Draw(pGraphicsAPI, false, m_MainCamera, m_Projection, m_DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}

		// Main FrameBuffer
		{
			if (!pGraphicsAPI->BeginRender()) return false;

			if (!pLoadWorker->Draw(pGraphicsAPI, false, m_MainCamera, m_Projection, m_DrawInfo)) return false;

			// GUIEngine
#ifdef USE_GUIENGINE
			if (pLoadWorker->IsLoaded())
			{
				if (!GUIEngine->BeginFrame(pGraphicsAPI)) return false;
				if (!m_GraphicsEditingWindow->Draw(pGraphicsAPI, this)) return false;
				if (!GUIEngine->EndFrame(pGraphicsAPI)) return false;
			}
#endif // USE_GUIENGINE

			if (!pGraphicsAPI->EndRender()) return false;
		}

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
		if (Focused && pLoadWorker)
		{
			m_FileModifier->OnFileUpdated(pLoadWorker);
		}
	}

	const std::shared_ptr<CFileModifier>& CScriptApp::GetFileModifier() const
	{
		return m_FileModifier;
	}

	const std::shared_ptr<scene::CSceneController>& CScriptApp::GetSceneController() const
	{
		return m_ScriptScene->GetSceneController();
	}
}