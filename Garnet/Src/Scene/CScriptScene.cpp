#include "CScriptScene.h"

#include "../../LoadWorker/CLoadWorker.h"
#include "../LoadWorker/CFile.h"
#include "../LoadWorker/CMaterialFrameLoader.h"
#include "../LoadWorker/CTextureLoader.h"

#include "../Debug/Message/Console.h"

#include "../Object/C3DObject.h"
#include "../Object/C3DObjectImporter.h"

#include "../GLTF/CGLTFImporter.h"
#include "../FBX/CFBXImporter.h"

#include "../Graphics/CDrawInfo.h"
#include "../Graphics/CMaterialFrame.h"

#include "../Camera/CCamera.h"

namespace scene
{
	CScriptScene::CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker):
		m_SampleMF(nullptr),
		m_PBRMF(nullptr),
		m_SimpleTextureMF(nullptr),
		m_DepthMF(nullptr),

		m_MfTestObject(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_glTFObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_BrainStemDObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_VRMObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_FbxObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		//m_glTFData(std::make_shared<resource::CFile>("Resources\\Models\\Sponza\\glTF\\Sponza.glb")),
		m_glTFData(std::make_shared<resource::CFile>("Resources\\Models\\CesiumMan\\glTF-Binary\\CesiumMan.glb")),

		m_BrainStemData(std::make_shared<resource::CFile>("Resources\\Models\\BrainStem\\glTF-Binary\\BrainStem.glb")),
		m_VRMData(std::make_shared<resource::CFile>("Resources\\Models\\Alicia\\VRM\\AliciaSolid.vrm")),

		//m_FbxAnimationData(std::make_shared<resource::CFile>("Resources\\Motions\\Walking.fbx")),
		m_FbxAnimationData(std::make_shared<resource::CFile>("Resources\\Motions\\Walking_WithSkin.fbx")),
		m_FbxData(std::make_shared<resource::CFile>("Resources\\Motions\\Walking_WithSkin.fbx")),

		m_Background(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_DebugSphere(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_IBL_Skybox_Texture(nullptr),
		m_IBL_DiffuseEnvMap_Texture(nullptr),
		m_IBL_SpecularEnvMap_Texture(nullptr),
		m_IBL_GGX_LUT_Texture(nullptr),
		m_Cube_Texture(nullptr),

		m_IsLoaded(false)
	{
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\Sample_MF.json", m_SampleMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\PBR_MF.json", m_PBRMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\SimpleTexture_MF.json", m_SimpleTextureMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\Depth_MF.json", m_DepthMF));
		pLoadWorker->AddFirstLoadResource(m_FbxAnimationData);
		pLoadWorker->AddFirstLoadResource(m_glTFData);
		pLoadWorker->AddFirstLoadResource(m_BrainStemData);
		pLoadWorker->AddFirstLoadResource(m_VRMData);

		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\IBL\\output_skybox.hdr" }), false, m_IBL_Skybox_Texture));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\IBL\\output_iem.hdr" }), false, m_IBL_DiffuseEnvMap_Texture));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\IBL\\output_pmrem.hdr" }), false, m_IBL_SpecularEnvMap_Texture));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\Textures\\ggx_lut.jpg" }), false, m_IBL_GGX_LUT_Texture));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\Cubemaps\\environment\\environment_back_0.jpg", "Resources\\Cubemaps\\environment\\environment_bottom_0.jpg", "Resources\\Cubemaps\\environment\\environment_front_0.jpg", "Resources\\Cubemaps\\environment\\environment_left_0.jpg", "Resources\\Cubemaps\\environment\\environment_right_0.jpg", "Resources\\Cubemaps\\environment\\environment_top_0.jpg" }), false, m_Cube_Texture));
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
			if (!fbx::CFBXImporter::ImportFBXAnimation(pGraphicsAPI, "Resources\\Motions\\Walking.fbx", AnimationClipList)) return false;
		}

		// glTFObject
		{
			std::shared_ptr<graphics::CTextureSet> TextureSet = std::make_shared<graphics::CTextureSet>();
			TextureSet->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { TextureSet->AddFrameTexture(FrameTexture); }
			TextureSet->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			m_glTFObject->SetPlayClipIndex(0);
			m_glTFObject->SetPos(glm::vec3(2.0f, 0.0f, 0.0f));

			if (!gltf::CGLTFImporter::ImportFromMemory(pGraphicsAPI, m_glTFData->GetData(), m_glTFObject, m_PBRMF, TextureSet, m_DepthMF)) return false;
		}
		
		{
			// TextureSet
			std::shared_ptr<graphics::CTextureSet> TextureSet = std::make_shared<graphics::CTextureSet>();
			TextureSet->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { TextureSet->AddFrameTexture(FrameTexture); }
			TextureSet->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			// 再生するアニメーションクリップを指定する
			m_BrainStemDObject->SetPlayClipIndex(0);

			m_BrainStemDObject->SetPos(glm::vec3(-2.0f, 0.0f, 0.0f));
			m_BrainStemDObject->SetRot(glm::angleAxis(3.1415f, glm::vec3(0.0f, 1.0f, 0.0f)));

			if (!gltf::CGLTFImporter::ImportFromMemory(pGraphicsAPI, m_BrainStemData->GetData(), m_BrainStemDObject, m_PBRMF, TextureSet, m_DepthMF)) return false;
		}
		
		{
			// TextureSet
			std::shared_ptr<graphics::CTextureSet> TextureSet = std::make_shared<graphics::CTextureSet>();
			TextureSet->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { TextureSet->AddFrameTexture(FrameTexture); }
			TextureSet->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			m_VRMObject->SetPos(glm::vec3(0.0f, 0.0f, 3.0f));

			if (!gltf::CGLTFImporter::ImportFromMemory(pGraphicsAPI, m_VRMData->GetData(), m_VRMObject, m_PBRMF, TextureSet, m_DepthMF)) return false;

			// 再生するアニメーションクリップを指定する
			m_VRMObject->SetPlayClipIndex(0);
			m_VRMObject->AddHumanoidAnimationClip(AnimationClipList[0]);
		}
		
		{
			// TextureSet
			std::shared_ptr<graphics::CTextureSet> TextureSet = std::make_shared<graphics::CTextureSet>();
			TextureSet->AddCubeMap(m_Cube_Texture);
			for(const auto& FrameTexture : m_FrameTextureList) { TextureSet->AddFrameTexture(FrameTexture); }
			TextureSet->AddIBLTexture(m_IBL_DiffuseEnvMap_Texture, m_IBL_SpecularEnvMap_Texture, m_IBL_GGX_LUT_Texture);

			// 再生するアニメーションクリップを指定する
			m_FbxObject->SetPlayClipIndex(1);

			m_FbxObject->SetRot(glm::angleAxis(3.1415f, glm::vec3(0.0f, 1.0f, 0.0f)));

			if (!fbx::CFBXImporter::ImportFBX(pGraphicsAPI, "Resources\\Motions\\Walking_WithSkin.fbx", m_FbxObject, m_PBRMF, TextureSet, m_DepthMF)) return false;
		}

		// m_MfTestObject
		{
			if (!object::C3DObjectImporter::ImportSimply(pGraphicsAPI, m_MfTestObject, graphics::CPresetPrimitive::CreateSphere(), m_SampleMF->CreateMaterial(pGraphicsAPI), std::make_shared<graphics::CTextureSet>(m_IBL_Skybox_Texture), m_DepthMF)) return false;
		}

		// m_Background
		{
			auto Mat = m_SimpleTextureMF->CreateMaterial(pGraphicsAPI);
			Mat->ReplacePreloadUniformValue("useDirSampling", &glm::ivec1(1)[0], sizeof(glm::ivec1), 1);
			Mat->ReplacePreloadUniformValue("useTexColor", &glm::ivec1(1)[0], sizeof(glm::ivec1), 1);
			Mat->ReplaceTextureIndex("texImage", 0);
			Mat->SetCullMode(graphics::ECullMode::CULL_FRONT);

			m_Background->SetScale(glm::vec3(500.0f));
			if (!object::C3DObjectImporter::ImportSimply(pGraphicsAPI, m_Background, graphics::CPresetPrimitive::CreateSphere(), Mat, std::make_shared<graphics::CTextureSet>(m_IBL_Skybox_Texture), m_DepthMF)) return false;
		}

		// m_DebugSphere
		{
			auto Mat = m_SimpleTextureMF->CreateMaterial(pGraphicsAPI);
			Mat->SetEnabledZTest(false);
			m_DebugSphere->SetScale(glm::vec3(0.1f));
			if (!object::C3DObjectImporter::ImportSimply(pGraphicsAPI, m_DebugSphere, graphics::CPresetPrimitive::CreateSphere(), Mat, nullptr, m_DepthMF)) return false;
		}

		return true;
	}

	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
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
			if (!m_MfTestObject->Draw(IsDepthPass, Camera, Projection, DrawInfo)) return false;
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