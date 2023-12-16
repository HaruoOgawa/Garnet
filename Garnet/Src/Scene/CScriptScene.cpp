#include "CScriptScene.h"

#include "../LoadWorker/CLoadWorker.h"
#include "../LoadWorker/CFile.h"
#include "../LoadWorker/CMaterialFrameLoader.h"
#include "../LoadWorker/CTextureLoader.h"
#include "../LoadWorker/C3DObjectLoader.h"

#include "../Debug/Message/Console.h"

#include "../Object/C3DObject.h"

#include "../Graphics/CMaterialFrame.h"

namespace scene
{
	CScriptScene::CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker):
		m_SampleMF(std::make_shared<graphics::CMaterialFrame>()),
		m_PBRMF(std::make_shared<graphics::CMaterialFrame>()),
		m_SimpleTextureMF(std::make_shared<graphics::CMaterialFrame>()),
		m_DepthMF(std::make_shared<graphics::CMaterialFrame>()),

		m_MfTestObject(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_glTFObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_BrainStemDObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_VRMObject(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_FbxAnimation(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_FbxObject(std::make_shared<object::C3DObject>("", "ShadowPass")),

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
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\SimpleTexture_MF.json", m_SimpleTextureMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\Depth_MF.json", m_DepthMF));
		
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Motions\\Walking_WithSkin.fbx", m_FbxObject, "", "ShadowPass"));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Motions\\Walking.fbx", m_FbxAnimation, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Models\\CesiumMan\\glTF-Binary\\CesiumMan.glb", m_glTFObject, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Models\\BrainStem\\glTF-Binary\\BrainStem.glb", m_BrainStemDObject, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Models\\Alicia\\VRM\\AliciaSolid.vrm", m_VRMObject, "", "ShadowPass"));
		
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

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI)
	{
		// FBX Humanoid Animation Clip
		std::vector<std::shared_ptr<animation::CAnimationClip>> AnimationClipList;
		{
			//if (!m_FbxAnimation->CreateFromMemory(pGraphicsAPI, nullptr, nullptr, object::E3DObjectType::Fbx)) return false;
			//AnimationClipList = m_FbxAnimation->GetAnimationClipList();
		}

		// glTFObject
		{
			m_glTFObject->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { m_glTFObject->GetTextureSet()->AddFrameTexture(FrameTexture); }
			m_glTFObject->GetTextureSet()->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			if (!m_glTFObject->CreateFromMemory(pGraphicsAPI, m_PBRMF, m_DepthMF, object::E3DObjectType::glTF)) return false;

			m_glTFObject->SetPlayClipIndex(0);
			m_glTFObject->SetPos(glm::vec3(2.0f, 0.0f, 0.0f));
		}
		
		{
			m_BrainStemDObject->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { m_BrainStemDObject->GetTextureSet()->AddFrameTexture(FrameTexture); }
			m_BrainStemDObject->GetTextureSet()->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			if (!m_BrainStemDObject->CreateFromMemory(pGraphicsAPI, m_PBRMF, m_DepthMF, object::E3DObjectType::glTF)) return false;

			// 再生するアニメーションクリップを指定する
			m_BrainStemDObject->SetPlayClipIndex(0);

			m_BrainStemDObject->SetPos(glm::vec3(-2.0f, 0.0f, 0.0f));
			m_BrainStemDObject->SetRot(glm::angleAxis(3.1415f, glm::vec3(0.0f, 1.0f, 0.0f)));
		}
		
		{
			m_VRMObject->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { m_VRMObject->GetTextureSet()->AddFrameTexture(FrameTexture); }
			m_VRMObject->GetTextureSet()->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			if (!m_VRMObject->CreateFromMemory(pGraphicsAPI, m_PBRMF, m_DepthMF, object::E3DObjectType::glTF)) return false;

			m_VRMObject->SetPos(glm::vec3(0.0f, 0.0f, 3.0f));
			// 再生するアニメーションクリップを指定する
			/*m_VRMObject->SetPlayClipIndex(0);
			m_VRMObject->AddHumanoidAnimationClip(AnimationClipList[1]);*/
		}
		
		{
			m_FbxObject->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { m_FbxObject->GetTextureSet()->AddFrameTexture(FrameTexture); }
			m_FbxObject->GetTextureSet()->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			if (!m_FbxObject->CreateFromMemory(pGraphicsAPI, m_PBRMF, m_DepthMF, object::E3DObjectType::Fbx)) return false;

			// 再生するアニメーションクリップを指定する
			m_FbxObject->SetPlayClipIndex(0);

			m_FbxObject->SetRot(glm::angleAxis(3.1415f, glm::vec3(0.0f, 1.0f, 0.0f)));
		}

		// m_MfTestObject
		{
			m_MfTestObject->GetTextureSet()->Add2DTexture(m_IBL_Skybox_Texture);
			if (!object::C3DObject::CreateSimply(pGraphicsAPI, m_MfTestObject, graphics::CPresetPrimitive::CreateSphere(), m_SampleMF->CreateMaterial(pGraphicsAPI), m_DepthMF)) return false;
		}

		// m_Background
		{
			auto Mat = m_SimpleTextureMF->CreateMaterial(pGraphicsAPI);
			Mat->ReplacePreloadUniformValue("useDirSampling", &glm::ivec1(1)[0], sizeof(glm::ivec1), 1);
			Mat->ReplacePreloadUniformValue("useTexColor", &glm::ivec1(1)[0], sizeof(glm::ivec1), 1);
			Mat->ReplaceTextureIndex("texImage", 0);
			Mat->SetCullMode(graphics::ECullMode::CULL_FRONT);

			m_Background->GetTextureSet()->Add2DTexture(m_IBL_Skybox_Texture);

			m_Background->SetScale(glm::vec3(500.0f));
			if (!object::C3DObject::CreateSimply(pGraphicsAPI, m_Background, graphics::CPresetPrimitive::CreateSphere(), Mat , m_DepthMF)) return false;
		}

		// m_DebugSphere
		{
			auto Mat = m_SimpleTextureMF->CreateMaterial(pGraphicsAPI);
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

			if (!Load(pGraphicsAPI)) return false;
			m_IsLoaded = true;
		}

		if (m_glTFObject)
		{
			if (!m_glTFObject->Update(DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_BrainStemDObject)
		{
			if (!m_BrainStemDObject->Update(DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_VRMObject)
		{
			if (!m_VRMObject->Update(DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_FbxObject)
		{
			if (!m_FbxObject->Update(DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_MfTestObject)
		{
			if (!m_MfTestObject->Update(DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_Background)
		{
			if (!m_Background->Update(DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_DebugSphere)
		{
			if (!m_DebugSphere->Update(DrawInfo->GetDeltaSecondsTime())) return false;
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
		
		if (m_glTFObject)
		{
			if (!m_glTFObject->Draw(IsDepthPass, Camera, Projection, DrawInfo, m_DebugSphere)) return false;
		}
		
		if (m_BrainStemDObject)
		{
			if (!m_BrainStemDObject->Draw(IsDepthPass, Camera, Projection, DrawInfo, m_DebugSphere)) return false;
		}
		
		if (m_VRMObject)
		{
			if (!m_VRMObject->Draw(IsDepthPass, Camera, Projection, DrawInfo, m_DebugSphere)) return false;
		}
		
		if (m_FbxObject)
		{
			if (!m_FbxObject->Draw(IsDepthPass, Camera, Projection, DrawInfo, m_DebugSphere)) return false;
		}
		
		if (m_MfTestObject)
		{
			//if (!m_MfTestObject->Draw(IsDepthPass, Camera, Projection, DrawInfo)) return false;
		}
		
		if (m_Background)
		{
			if (!m_Background->Draw(IsDepthPass, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	// Tex of FrameBuffer
	void CScriptScene::SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
		m_FrameTextureList.push_back(FrameTexture);
	}
}