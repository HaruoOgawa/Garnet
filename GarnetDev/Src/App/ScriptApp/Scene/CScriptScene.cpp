#include "CScriptScene.h"

#include <LoadWorker/CLoadWorker.h>
#include <LoadWorker/CFile.h>
#include <LoadWorker/CMaterialFrameLoader.h>
#include <LoadWorker/CTextureLoader.h>
#include <LoadWorker/C3DObjectLoader.h>
#include <LoadWorker/CAnimationLoader.h>
#include <LoadWorker/CAudioLoader.h>

#include <Message/Console.h>

#include <Object/C3DObject.h>
#include <Animation/CAnimationClipSet.h>
#include <Graphics/CMaterialFrame.h>

#include <Scene/CSceneController.h>

namespace app
{
	CScriptScene::CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, physics::IPhysicsEngine* pPhysicsEngine):
		m_IsLoaded(false),
		m_SceneController(std::make_shared<scene::CSceneController>())
	{
		pLoadWorker->AddScene(std::make_shared<resource::CSceneLoader>("Resources\\Scene\\MRTTest.json", m_SceneController));
	}

	CScriptScene::~CScriptScene()
	{
		m_IsLoaded = false;
	}

	bool CScriptScene::IsLoaded() const
	{
		return m_IsLoaded;
	}

	const std::shared_ptr<scene::CSceneController>& CScriptScene::GetSceneController() const
	{
		return m_SceneController;
	}

	std::vector<std::shared_ptr<object::C3DObject>> CScriptScene::GetObjectList() const
	{
		std::vector<std::shared_ptr<object::C3DObject>> ObjectList;

		for (const auto& Object : m_SceneController->GetObjectList())
		{
			ObjectList.push_back(Object);
		}

		for (const auto& Object : m_ObjectList)
		{
			ObjectList.push_back(Object);
		}

		return ObjectList;
	}

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		if (!m_SceneController->Create(pGraphicsAPI, pPhysicsEngine)) return false;

		return true;
	}

	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_IsLoaded) return true;

		if (!m_SceneController->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		
		return true;
	}

	bool CScriptScene::LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		if (!m_SceneController->LateUpdate(pGraphicsAPI, pPhysicsEngine, pLoadWorker, DrawInfo)) return false;

		return true;
	}

	bool CScriptScene::FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;
		
		if (!m_SceneController->FixedUpdate(pGraphicsAPI, pPhysicsEngine, pLoadWorker, DrawInfo)) return false;

		return true;
	}

	bool CScriptScene::Dispatch(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		if (!m_SceneController->Draw(pGraphicsAPI, false, Camera, Projection, DrawInfo)) return false;

		return true;
	}

	// Tex of FrameBuffer
	void CScriptScene::SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
	}

	// ロード完了イベント
	bool CScriptScene::OnLoaded(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		if (!Load(pGraphicsAPI, pPhysicsEngine, pLoadWorker)) return false;
		m_IsLoaded = true;

		return true;
	}
}