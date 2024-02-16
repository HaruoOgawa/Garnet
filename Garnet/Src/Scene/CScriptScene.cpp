#include "CScriptScene.h"

#include "../LoadWorker/CLoadWorker.h"
#include "../LoadWorker/CFile.h"
#include "../LoadWorker/CMaterialFrameLoader.h"
#include "../LoadWorker/CTextureLoader.h"
#include "../LoadWorker/C3DObjectLoader.h"
#include "../LoadWorker/CAnimationLoader.h"

#include "../Debug/Message/Console.h"

#include "../Object/C3DObject.h"
#include "../Animation/CAnimationClipSet.h"

#include "../Graphics/CMaterialFrame.h"

namespace scene
{
	CScriptScene::CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker):
		m_BasicToonMF(std::make_shared<graphics::CMaterialFrame>()),
		m_SimpleTextureMF(std::make_shared<graphics::CMaterialFrame>()),
		m_DepthMF(std::make_shared<graphics::CMaterialFrame>()),

		m_TdaMiku_Model(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_VMDAnimationSet(std::make_shared<animation::CAnimationClipSet>()),

		m_Background(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_IBL_Skybox_Texture(pGraphicsAPI->CreateTexture(false)),

		m_IsLoaded(false)
	{
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\basic_toon_mf.json", m_BasicToonMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\SimpleTexture_MF.json", m_SimpleTextureMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\Depth_MF.json", m_DepthMF));
		
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Avatar\\Tda_Miku\\Tda_Miku.pmx", m_TdaMiku_Model, "", "ShadowPass"));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\mmd_running.vmd", m_VMDAnimationSet));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\Run_m4th_Loop.vmd", m_VMDAnimationSet));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\BackFlip.vmd", m_VMDAnimationSet));
		
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\IBL\\output_skybox.hdr" }), m_IBL_Skybox_Texture));
	}

	CScriptScene::~CScriptScene()
	{
		m_IsLoaded = false;
	}

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		// m_TdaMiku_Model
		{
			if (!m_TdaMiku_Model->CreateFromMemory(pGraphicsAPI, pLoadWorker, m_BasicToonMF, m_DepthMF, object::E3DObjectType::Pmx)) return false;

			m_TdaMiku_Model->SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
			m_TdaMiku_Model->SetScale(glm::vec3(0.1f));

			auto Clip = m_VMDAnimationSet->GetAnimationClip(0);
			if (Clip) m_TdaMiku_Model->AddHumanoidAnimationClip(Clip, "Walk", { nullptr, "" }, true);

			m_TdaMiku_Model->ChangeMotion("Walk");
		}

		// m_Background
		{
			auto Mat = m_SimpleTextureMF->CreateMaterial(pGraphicsAPI, 1, graphics::ECullMode::CULL_FRONT);
			Mat->ReplacePreloadUniformValue("useDirSampling", &glm::ivec1(1)[0], sizeof(glm::ivec1), 1);
			Mat->ReplacePreloadUniformValue("useTexColor", &glm::ivec1(1)[0], sizeof(glm::ivec1), 1);
			Mat->ReplaceTextureIndex("texImage", 0);

			m_Background->GetTextureSet()->Add2DTexture(m_IBL_Skybox_Texture);

			m_Background->SetScale(glm::vec3(500.0f));
			if (!object::C3DObject::CreateSimply(pGraphicsAPI, m_Background, graphics::CPresetPrimitive::CreateSphere(), Mat , m_DepthMF)) return false;
		}

		return true;
	}

#ifdef USE_INPUT_SYSTEM
	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
#else
	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
#endif
	{
		if (!m_IsLoaded)
		{
			if (!pLoadWorker->IsLoaded()) return true;

			if (!Load(pGraphicsAPI, pLoadWorker)) return false;
			m_IsLoaded = true;
		}

		if (m_TdaMiku_Model)
		{
			if (!m_TdaMiku_Model->Update(pGraphicsAPI, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_Background)
		{
			if (!m_Background->Update(pGraphicsAPI, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}

	bool CScriptScene::Dispatch(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;
		
		if (m_TdaMiku_Model)
		{
			if (!m_TdaMiku_Model->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
			//if (!m_TdaMiku_Model->Draw(IsDepthPass, true, Camera, Projection, DrawInfo, nullptr)) return false;
		}
		
		if (m_Background)
		{
			if (!m_Background->Draw(IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	// Tex of FrameBuffer
	void CScriptScene::SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
	}
}