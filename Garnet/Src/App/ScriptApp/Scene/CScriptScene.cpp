#include "CScriptScene.h"

#include "../../../LoadWorker/CLoadWorker.h"
#include "../../../LoadWorker/CFile.h"
#include "../../../LoadWorker/CMaterialFrameLoader.h"
#include "../../../LoadWorker/CTextureLoader.h"
#include "../../../LoadWorker/C3DObjectLoader.h"
#include "../../../LoadWorker/CAnimationLoader.h"
#include "../../../LoadWorker/CAudioLoader.h"

#include "../../../Message/Console.h"

#include "../../../Object/C3DObject.h"
#include "../../../Animation/CAnimationClipSet.h"
#include "../../../Graphics/CMaterialFrame.h"

namespace app
{
	CScriptScene::CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, physics::IPhysicsEngine* pPhysicsEngine):
		m_BasicToonMF(std::make_shared<graphics::CMaterialFrame>()),
		m_SimpleTextureMF(std::make_shared<graphics::CMaterialFrame>()),
		m_DepthMF(std::make_shared<graphics::CMaterialFrame>()),
		m_PBRMF(std::make_shared<graphics::CMaterialFrame>()),
		m_RaymarchingMF(std::make_shared<graphics::CMaterialFrame>()),

		m_TdaMiku_Model(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_VMDAnimationSet(std::make_shared<animation::CAnimationClipSet>()),
		m_ExpressionVMD(std::make_shared<animation::CAnimationClipSet>()),
		m_RipSyncVMD(std::make_shared<animation::CAnimationClipSet>()),

		m_IBL_Skybox_Texture(pGraphicsAPI->CreateTexture(false)),
		m_Cube_Texture(pGraphicsAPI->CreateTexture(false)),

		m_RaymarchingObj(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_Background(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_DebugSphere(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_IsLoaded(false)
	{
		m_TdaMiku_Model->SetObjectName("TdaMiku_Model");
		m_RaymarchingObj->SetObjectName("Raymarching");
		m_Background->SetObjectName("Background");

		m_ObjectList.push_back(m_TdaMiku_Model);
		m_ObjectList.push_back(m_RaymarchingObj);
		m_ObjectList.push_back(m_Background);

		pLoadWorker->AddLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\basic_toon_mf.json", m_BasicToonMF));
		pLoadWorker->AddLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\SimpleTexture_MF.json", m_SimpleTextureMF));
		pLoadWorker->AddLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\Depth_MF.json", m_DepthMF));
		pLoadWorker->AddLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\PBR_MF.json", m_PBRMF));
		pLoadWorker->AddLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\raymarching_mf.json", m_RaymarchingMF));
		
		pLoadWorker->AddLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Avatar\\Tda_Miku\\Tda_Miku.pmx", m_TdaMiku_Model, m_BasicToonMF));
		pLoadWorker->AddLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\biglove_dance_noik.vmd", m_VMDAnimationSet));
		pLoadWorker->AddLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\biglove_expression.vmd", m_ExpressionVMD));
		pLoadWorker->AddLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\biglove_Ripsync.vmd", m_RipSyncVMD));
		
		pLoadWorker->AddLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\Cubemaps\\environment\\environment_back_0.jpg", "Resources\\Cubemaps\\environment\\environment_bottom_0.jpg", "Resources\\Cubemaps\\environment\\environment_front_0.jpg", "Resources\\Cubemaps\\environment\\environment_left_0.jpg", "Resources\\Cubemaps\\environment\\environment_right_0.jpg", "Resources\\Cubemaps\\environment\\environment_top_0.jpg" }), m_Cube_Texture));
		pLoadWorker->AddLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\IBL\\output_skybox.hdr" }), m_IBL_Skybox_Texture));
	}

	CScriptScene::~CScriptScene()
	{
		m_IsLoaded = false;
	}

	bool CScriptScene::IsLoaded() const
	{
		return m_IsLoaded;
	}

	const std::vector<std::shared_ptr<object::C3DObject>>& CScriptScene::GetObjectList() const
	{
		return m_ObjectList;
	}

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		// m_TdaMiku_Model
		{
			m_TdaMiku_Model->SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
			m_TdaMiku_Model->SetScale(glm::vec3(0.1f));

			if (!m_TdaMiku_Model->Create(pGraphicsAPI, pPhysicsEngine, m_DepthMF)) return false;

			auto Clip = m_VMDAnimationSet->GetAnimationClip(0);
			if (Clip) m_TdaMiku_Model->AddHumanoidAnimationClip(Clip, "Walk", { nullptr, "" }, true, false);

			//auto ExpressionClip = m_VMDAnimationSet->GetBlendShapeClip(0);
			auto ExpressionClip = m_ExpressionVMD->GetBlendShapeClip(0);
			if (ExpressionClip) m_TdaMiku_Model->AddBlendShapeClip(ExpressionClip, "Face", true);

			auto RipSyncClip = m_RipSyncVMD->GetBlendShapeClip(0);
			if (RipSyncClip) m_TdaMiku_Model->AddBlendShapeClip(RipSyncClip, "RipSync", true);

			m_TdaMiku_Model->ChangeMotion("Walk");
			m_TdaMiku_Model->PlayBlendShape("Face");
			m_TdaMiku_Model->PlayBlendShape("RipSync");
		}

		// m_RaymarchingObj
		{
			m_RaymarchingObj->GetTextureSet()->AddCubeMap(m_Cube_Texture);

			auto Material = m_RaymarchingMF->CreateMaterial(pGraphicsAPI, 2, graphics::ECullMode::CULL_NONE);
			//Material->ReplacePreloadUniformValue("useCubeMap", &glm::ivec1(1)[0], sizeof(glm::ivec1), 0);
			//Material->ReplaceTextureIndex("cubemapTexture", 0);

			Material->SetEnabledZWrite(false);

			if (!m_RaymarchingObj->CreateSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateBoard(pGraphicsAPI), Material, m_DepthMF)) return false;
		}

		// m_Background
		{
			auto Mat = m_SimpleTextureMF->CreateMaterial(pGraphicsAPI, 1, graphics::ECullMode::CULL_FRONT);
			Mat->ReplacePreloadUniformValue("useDirSampling", &glm::ivec1(1)[0], sizeof(glm::ivec1), 1);
			Mat->ReplacePreloadUniformValue("useTexColor", &glm::ivec1(1)[0], sizeof(glm::ivec1), 1);
			Mat->ReplaceTextureIndex("texImage", 0);

			m_Background->GetTextureSet()->Add2DTexture(m_IBL_Skybox_Texture);

			m_Background->SetScale(glm::vec3(500.0f));

			m_Background->SetEnabled(false);

			if (!m_Background->CreateSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateSphere(pGraphicsAPI), Mat , m_DepthMF)) return false;
		}

		// m_DebugSphere
		{
			auto Mat = m_PBRMF->CreateMaterial(pGraphicsAPI, 512, graphics::ECullMode::CULL_BACK);
			Mat->SetDepthFunc(graphics::EDepthFunc::Always);
			m_DebugSphere->SetScale(glm::vec3(0.1f));
			if (!m_DebugSphere->CreateSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateSphere(pGraphicsAPI), Mat, m_DepthMF)) return false;
		}

		return true;
	}

	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_IsLoaded)
		{
			if (!pLoadWorker->IsLoaded()) return true;

			if (!Load(pGraphicsAPI, pPhysicsEngine, pLoadWorker)) return false;
			m_IsLoaded = true;
		}

		if (m_TdaMiku_Model)
		{
			if (!m_TdaMiku_Model->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_RaymarchingObj)
		{
			if (!m_RaymarchingObj->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_Background)
		{
			if (!m_Background->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		if (m_DebugSphere)
		{
			if (!m_DebugSphere->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}

	bool CScriptScene::LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		return true;
	}

	bool CScriptScene::FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		if (m_TdaMiku_Model)
		{
			if (!m_TdaMiku_Model->FixedUpdate(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

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
		
		if (m_RaymarchingObj)
		{
			if (!m_RaymarchingObj->Draw(IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
		}

		if (m_Background)
		{
			if (!m_Background->Draw(IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
		}

		if (m_TdaMiku_Model)
		{
			if (!m_TdaMiku_Model->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, m_DebugSphere)) return false;
			//if (!m_TdaMiku_Model->Draw(IsDepthPass, true, Camera, Projection, DrawInfo, nullptr)) return false;
		}

		return true;
	}

	// Tex of FrameBuffer
	void CScriptScene::SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
	}
}