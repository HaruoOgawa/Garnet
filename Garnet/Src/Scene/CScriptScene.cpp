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
		m_SampleMF(std::make_shared<graphics::CMaterialFrame>()),
		m_PBRMF(std::make_shared<graphics::CMaterialFrame>()),
		m_BasicToonMF(std::make_shared<graphics::CMaterialFrame>()),
		m_SimpleTextureMF(std::make_shared<graphics::CMaterialFrame>()),
		m_DepthMF(std::make_shared<graphics::CMaterialFrame>()),

		m_TdaMiku_Model(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_VMDAnimationSet(std::make_shared<animation::CAnimationClipSet>()),

		m_glTFObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_BrainStemDObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_VRMObject(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_Walk_Animation(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_Jump_Animation(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_Punch_Animation(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_FbxObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_MouseyObject(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_Background(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_DebugSphere(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_IBL_Skybox_Texture(pGraphicsAPI->CreateTexture(false)),
		m_IBL_DiffuseEnvMap_Texture(pGraphicsAPI->CreateTexture(false)),
		m_IBL_SpecularEnvMap_Texture(pGraphicsAPI->CreateTexture(false)),
		m_IBL_GGX_LUT_Texture(pGraphicsAPI->CreateTexture(false)),
		m_Cube_Texture(pGraphicsAPI->CreateTexture(false)),

		m_IsLoaded(false)
	{
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\Sample_MF.json", m_SampleMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\PBR_MF.json", m_PBRMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\basic_toon_mf.json", m_BasicToonMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\SimpleTexture_MF.json", m_SimpleTextureMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\Depth_MF.json", m_DepthMF));
		
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Avatar\\Tda_Miku\\Tda_Miku.pmx", m_TdaMiku_Model, "", "ShadowPass"));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\mmd_running.vmd", m_VMDAnimationSet));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\Run_m4th_Loop.vmd", m_VMDAnimationSet));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\BackFlip.vmd", m_VMDAnimationSet));

		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Avatar\\X_Bot.fbx", m_FbxObject, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Avatar\\Mousey.fbx", m_MouseyObject, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Motions\\Walking.fbx", m_Walk_Animation, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Motions\\Jumping.fbx", m_Jump_Animation, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Motions\\Combo_Punch_hand.fbx", m_Punch_Animation, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Models\\CesiumMan\\glTF-Binary\\CesiumMan.glb", m_glTFObject, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Models\\BrainStem\\glTF-Binary\\BrainStem.glb", m_BrainStemDObject, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Avatar\\Warrok_W_Kurniawan.fbx", m_VRMObject, "", "ShadowPass"));
		
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\IBL\\output_skybox.hdr" }), m_IBL_Skybox_Texture));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\IBL\\output_iem.hdr" }), m_IBL_DiffuseEnvMap_Texture));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\IBL\\output_pmrem.hdr" }), m_IBL_SpecularEnvMap_Texture));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\Textures\\ggx_lut.jpg" }), m_IBL_GGX_LUT_Texture));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\Cubemaps\\environment\\environment_back_0.jpg", "Resources\\Cubemaps\\environment\\environment_bottom_0.jpg", "Resources\\Cubemaps\\environment\\environment_front_0.jpg", "Resources\\Cubemaps\\environment\\environment_left_0.jpg", "Resources\\Cubemaps\\environment\\environment_right_0.jpg", "Resources\\Cubemaps\\environment\\environment_top_0.jpg" }), m_Cube_Texture));
	}

	CScriptScene::~CScriptScene()
	{
		m_IsLoaded = false;
	}

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		// FBX Humanoid Animation Clip
		std::vector<std::shared_ptr<animation::CAnimationClip>> AnimationClipList;
		{
			if (!m_Walk_Animation->CreateFromMemory(pGraphicsAPI, pLoadWorker, nullptr, nullptr, object::E3DObjectType::Fbx)) return false;
			if (!m_Jump_Animation->CreateFromMemory(pGraphicsAPI, pLoadWorker, nullptr, nullptr, object::E3DObjectType::Fbx)) return false;
			if (!m_Punch_Animation->CreateFromMemory(pGraphicsAPI, pLoadWorker, nullptr, nullptr, object::E3DObjectType::Fbx)) return false;

			AnimationClipList.push_back(m_Walk_Animation->GetAnimationClipList()[0]);
			AnimationClipList.push_back(m_Jump_Animation->GetAnimationClipList()[0]);
			AnimationClipList.push_back(m_Punch_Animation->GetAnimationClipList()[0]);
		}

		// m_TdaMiku_Model
		{
			if (!m_TdaMiku_Model->CreateFromMemory(pGraphicsAPI, pLoadWorker, m_BasicToonMF, m_DepthMF, object::E3DObjectType::Pmx)) return false;

			m_TdaMiku_Model->SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
			m_TdaMiku_Model->SetScale(glm::vec3(0.1f));

			auto Clip = m_VMDAnimationSet->GetAnimationClip(0);
			if (Clip) m_TdaMiku_Model->AddHumanoidAnimationClip(Clip, "Walk", { nullptr, "" }, true);

			m_TdaMiku_Model->ChangeMotion("Walk");
		}

		// glTFObject
		{
			m_glTFObject->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { m_glTFObject->GetTextureSet()->AddFrameTexture(FrameTexture); }
			m_glTFObject->GetTextureSet()->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			if (!m_glTFObject->CreateFromMemory(pGraphicsAPI, pLoadWorker, m_PBRMF, m_DepthMF, object::E3DObjectType::glTF)) return false;

			m_glTFObject->ChangeMotion(0);
			m_glTFObject->SetPos(glm::vec3(4.0f, 0.0f, 0.0f));
		}
		
		{
			m_BrainStemDObject->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { m_BrainStemDObject->GetTextureSet()->AddFrameTexture(FrameTexture); }
			m_BrainStemDObject->GetTextureSet()->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			if (!m_BrainStemDObject->CreateFromMemory(pGraphicsAPI, pLoadWorker, m_PBRMF, m_DepthMF, object::E3DObjectType::glTF)) return false;

			// 再生するアニメーションクリップを指定する
			m_BrainStemDObject->ChangeMotion(0);

			m_BrainStemDObject->SetPos(glm::vec3(2.0f, 0.0f, 0.0f));
			m_BrainStemDObject->SetRot(glm::angleAxis(3.1415f, glm::vec3(0.0f, 1.0f, 0.0f)));
		}
		
		{
			m_VRMObject->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { m_VRMObject->GetTextureSet()->AddFrameTexture(FrameTexture); }
			m_VRMObject->GetTextureSet()->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			//if (!m_VRMObject->CreateFromMemory(pGraphicsAPI, pLoadWorker, m_PBRMF, m_DepthMF, object::E3DObjectType::glTF)) return false;
			if (!m_VRMObject->CreateFromMemory(pGraphicsAPI, pLoadWorker, m_PBRMF, m_DepthMF, object::E3DObjectType::Fbx)) return false;

			m_VRMObject->AddHumanoidAnimationClip(AnimationClipList[0], "Walk", { nullptr, "" }, true);
			m_VRMObject->AddHumanoidAnimationClip(AnimationClipList[1], "Jump", { nullptr, "Walk" }, false);
			m_VRMObject->AddHumanoidAnimationClip(AnimationClipList[2], "Punch", { nullptr, "Walk" }, false);

			m_VRMObject->SetPos(glm::vec3(-4.0f, 0.0f, 0.0f));
			m_VRMObject->SetRot(glm::angleAxis(3.1515f, glm::vec3(0.0f, 1.0f, 0.0f)));
			m_VRMObject->SetScale(glm::vec3(0.01f));

			// 再生するアニメーションクリップを指定する
			m_VRMObject->ChangeMotion("Walk");
		}
		
		{
			m_FbxObject->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { m_FbxObject->GetTextureSet()->AddFrameTexture(FrameTexture); }
			m_FbxObject->GetTextureSet()->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			if (!m_FbxObject->CreateFromMemory(pGraphicsAPI, pLoadWorker, m_PBRMF, m_DepthMF, object::E3DObjectType::Fbx)) return false;

			m_FbxObject->AddHumanoidAnimationClip(AnimationClipList[0], "Walk", { nullptr, "" }, true);
			m_FbxObject->AddHumanoidAnimationClip(AnimationClipList[1], "Jump", { nullptr, "Walk" }, false);
			m_FbxObject->AddHumanoidAnimationClip(AnimationClipList[2], "Punch", { nullptr, "Walk" }, false);
			
			// 再生するアニメーションクリップを指定する
			m_FbxObject->ChangeMotion("Walk");
			m_FbxObject->SetPos(glm::vec3(0.0f, 0.0f, 1.0f));
			m_FbxObject->SetRot(glm::angleAxis(3.1515f, glm::vec3(0.0f, 1.0f, 0.0f)));
			m_FbxObject->SetScale(glm::vec3(0.01f));
		}
		
		{
			m_MouseyObject->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { m_MouseyObject->GetTextureSet()->AddFrameTexture(FrameTexture); }
			m_MouseyObject->GetTextureSet()->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			if (!m_MouseyObject->CreateFromMemory(pGraphicsAPI, pLoadWorker, m_PBRMF, m_DepthMF, object::E3DObjectType::Fbx)) return false;

			m_MouseyObject->AddHumanoidAnimationClip(AnimationClipList[0], "Walk", { nullptr, "" }, true);
			m_MouseyObject->AddHumanoidAnimationClip(AnimationClipList[1], "Jump", { nullptr, "Walk" }, false);
			m_MouseyObject->AddHumanoidAnimationClip(AnimationClipList[2], "Punch", { nullptr, "Walk" }, false);
			
			// 再生するアニメーションクリップを指定する
			m_MouseyObject->ChangeMotion("Walk");

			m_MouseyObject->SetPos(glm::vec3(-2.0f, 0.0f, 0.0f));
			m_MouseyObject->SetRot(glm::angleAxis(3.1515f, glm::vec3(0.0f, 1.0f, 0.0f)));
			m_MouseyObject->SetScale(glm::vec3(0.01f));
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

		// m_DebugSphere
		{
			auto Mat = m_SimpleTextureMF->CreateMaterial(pGraphicsAPI, 512, graphics::ECullMode::CULL_BACK);
			Mat->SetEnabledZTest(false);
			m_DebugSphere->SetScale(glm::vec3(0.1f));
			if (!object::C3DObject::CreateSimply(pGraphicsAPI, m_DebugSphere, graphics::CPresetPrimitive::CreateSphere(), Mat, m_DepthMF)) return false;
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

		if (InputState->IsKeyUp(input::EKeyType::KEY_TYPE_1))
		{
			m_FbxObject->ChangeMotion("Jump");
			m_MouseyObject->ChangeMotion("Jump");
			m_VRMObject->ChangeMotion("Jump");
		}
		else if (InputState->IsKeyUp(input::EKeyType::KEY_TYPE_2))
		{
			m_FbxObject->ChangeMotion("Punch");
			m_MouseyObject->ChangeMotion("Punch");
			m_VRMObject->ChangeMotion("Punch");
		}

		if (m_TdaMiku_Model)
		{
			if (!m_TdaMiku_Model->Update(pGraphicsAPI, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_glTFObject)
		{
			if (!m_glTFObject->Update(pGraphicsAPI, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_BrainStemDObject)
		{
			if (!m_BrainStemDObject->Update(pGraphicsAPI, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_VRMObject)
		{
			if (!m_VRMObject->Update(pGraphicsAPI, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_FbxObject)
		{
			if (!m_FbxObject->Update(pGraphicsAPI, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_MouseyObject)
		{
			if (!m_MouseyObject->Update(pGraphicsAPI, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_Background)
		{
			if (!m_Background->Update(pGraphicsAPI, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_DebugSphere)
		{
			if (!m_DebugSphere->Update(pGraphicsAPI, DrawInfo->GetDeltaSecondsTime())) return false;
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
			//if (!m_TdaMiku_Model->Draw(IsDepthPass, true, Camera, Projection, DrawInfo, m_DebugSphere)) return false;
		}
		
		if (m_glTFObject)
		{
			if (!m_glTFObject->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
		}
		
		if (m_BrainStemDObject)
		{
			if (!m_BrainStemDObject->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
		}
		
		if (m_VRMObject)
		{
			if (!m_VRMObject->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
		}
		
		if (m_FbxObject)
		{
			if (!m_FbxObject->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
		}
		
		if (m_MouseyObject)
		{
			if (!m_MouseyObject->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
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
		m_FrameTextureList.push_back(FrameTexture);
	}
}