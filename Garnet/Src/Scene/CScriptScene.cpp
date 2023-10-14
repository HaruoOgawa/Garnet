#include "CScriptScene.h"
#include "../../LoadWorker/CLoadWorker.h"
#include "../Object/C3DObject.h"
#include "../File/CFile.h"
#include "../Debug/Message/Console.h"
#include "../GLTF/CGLTFImporter.h"
#include <glm/glm.hpp>
#include "../Graphics/CDrawInfo.h"
#include "../Camera/CCamera.h"

namespace scene
{
	CScriptScene::CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker):
		m_IsDrawSponza(false),
		m_CoolTime(0.0f),

		m_glTFObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		//m_glTFData(std::make_shared<file::CFile>("Resources\\Models\\MetalRoughSpheresNoTextures\\glTF-Binary\\MetalRoughSpheresNoTextures.glb")),
		//m_glTFData(std::make_shared<file::CFile>("Resources\\Models\\Sponza\\glTF\\Sponza.glb")),
		m_glTFData(std::make_shared<file::CFile>("Resources\\Models\\DamagedHelmet\\glTF-Binary\\DamagedHelmet.glb")),
		
		m_SponzaObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_SponzaData(std::make_shared<file::CFile>("Resources\\Models\\Sponza\\glTF\\Sponza.glb")),

		m_Background(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_IBL_Skybox(std::make_shared<file::CFile>("Resources\\IBL\\output_skybox.hdr")),
		m_IBL_DiffuseEnvMap(std::make_shared<file::CFile>("Resources\\IBL\\output_iem.hdr")),
		m_IBL_SpecularEnvMap(std::make_shared<file::CFile>("Resources\\IBL\\output_pmrem.hdr")),
		m_IBL_GGX_LUT(std::make_shared<file::CFile>("Resources\\Textures\\ggx_lut.jpg")),

		m_Cube0(std::make_shared<file::CFile>("Resources\\Cubemaps\\environment\\environment_back_0.jpg")),
		m_Cube1(std::make_shared<file::CFile>("Resources\\Cubemaps\\environment\\environment_bottom_0.jpg")),
		m_Cube2(std::make_shared<file::CFile>("Resources\\Cubemaps\\environment\\environment_front_0.jpg")),
		m_Cube3(std::make_shared<file::CFile>("Resources\\Cubemaps\\environment\\environment_left_0.jpg")),
		m_Cube4(std::make_shared<file::CFile>("Resources\\Cubemaps\\environment\\environment_right_0.jpg")),
		m_Cube5(std::make_shared<file::CFile>("Resources\\Cubemaps\\environment\\environment_top_0.jpg")),

		m_DepthVertex(std::make_shared<file::CFile>("Resources\\Shaders\\depth" + pGraphicsAPI->GetVertexShaderExtension())),
		m_DepthFragment(std::make_shared<file::CFile>("Resources\\Shaders\\depth" + pGraphicsAPI->GetFragmentShaderExtension())),

		m_VertexShader(std::make_shared<file::CFile>("Resources\\Shaders\\pbr" + pGraphicsAPI->GetVertexShaderExtension())),
		m_FragmentShader(std::make_shared<file::CFile>("Resources\\Shaders\\pbr" + pGraphicsAPI->GetFragmentShaderExtension())),
		m_MinimumVert(std::make_shared<file::CFile>("Resources\\Shaders\\minimum" + pGraphicsAPI->GetVertexShaderExtension())),
		m_TextureFrag(std::make_shared<file::CFile>("Resources\\Shaders\\texture" + pGraphicsAPI->GetFragmentShaderExtension())),

		m_IsLoaded(false)
	{
		pLoadWorker->AddFirstLoadResource(m_DepthVertex);
		pLoadWorker->AddFirstLoadResource(m_DepthFragment);
		pLoadWorker->AddFirstLoadResource(m_glTFData);
		pLoadWorker->AddFirstLoadResource(m_SponzaData);
		pLoadWorker->AddFirstLoadResource(m_IBL_Skybox);
		pLoadWorker->AddFirstLoadResource(m_IBL_DiffuseEnvMap);
		pLoadWorker->AddFirstLoadResource(m_IBL_SpecularEnvMap);
		pLoadWorker->AddFirstLoadResource(m_IBL_GGX_LUT);
		pLoadWorker->AddFirstLoadResource(m_VertexShader);
		pLoadWorker->AddFirstLoadResource(m_FragmentShader);
		pLoadWorker->AddFirstLoadResource(m_MinimumVert);
		pLoadWorker->AddFirstLoadResource(m_TextureFrag);
		pLoadWorker->AddFirstLoadResource(m_Cube0);
		pLoadWorker->AddFirstLoadResource(m_Cube1);
		pLoadWorker->AddFirstLoadResource(m_Cube2);
		pLoadWorker->AddFirstLoadResource(m_Cube3);
		pLoadWorker->AddFirstLoadResource(m_Cube4);
		pLoadWorker->AddFirstLoadResource(m_Cube5);
	}

	CScriptScene::~CScriptScene()
	{
		m_IsLoaded = false;
	}

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI)
	{
		// Cubemap
		std::vector<std::vector<unsigned char>> CubeDataList;
		CubeDataList.push_back(m_Cube0->GetData());
		CubeDataList.push_back(m_Cube1->GetData());
		CubeDataList.push_back(m_Cube2->GetData());
		CubeDataList.push_back(m_Cube3->GetData());
		CubeDataList.push_back(m_Cube4->GetData());
		CubeDataList.push_back(m_Cube5->GetData());

		auto CubeTex = pGraphicsAPI->CreateTexture(true);
		if (!CubeTex->Create(CubeDataList)) return false;

		// IBL
		auto IBL_Skybox_Tex = pGraphicsAPI->CreateTexture(false);
		if (!IBL_Skybox_Tex->Create(m_IBL_Skybox->GetData())) return false;
		
		auto IBL_Diffuse_Tex = pGraphicsAPI->CreateTexture(false);
		if (!IBL_Diffuse_Tex->Create(m_IBL_DiffuseEnvMap->GetData())) return false;

		auto IBL_Specular_Tex = pGraphicsAPI->CreateTexture(false);
		if (!IBL_Specular_Tex->Create(m_IBL_SpecularEnvMap->GetData())) return false;

		auto IBL_GGXLUT_Tex = pGraphicsAPI->CreateTexture(false);
		if (!IBL_GGXLUT_Tex->Create(m_IBL_GGX_LUT->GetData())) return false;

		// m_SponzaObject
		{
			// TRS
			m_SponzaObject->SetRot(glm::vec3(0.0f, 3.1415f * -0.5f, 0.0f)); // Sponza

			// MaterialInto
			std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
			createInfo->SetVertexShaderCode(m_VertexShader->GetData());
			createInfo->SetFragmentShaderCode(m_FragmentShader->GetData());

			// TextureSet
			std::shared_ptr<graphics::CTextureSet> TextureSet = std::make_shared<graphics::CTextureSet>();
			TextureSet->AddCubeMap(CubeTex);
			for (const auto& FrameTexture : m_FrameTextureList) { TextureSet->AddFrameTexture(FrameTexture); }
			TextureSet->AddIBLTexture(IBL_Diffuse_Tex, IBL_Specular_Tex, IBL_GGXLUT_Tex);

			// Import
			if (!gltf::CGLTFImporter::Import(pGraphicsAPI, m_SponzaData->GetData(), m_SponzaObject, createInfo, TextureSet, m_DepthVertex, m_DepthFragment)) return false;
		}

		// glTFObject
		{
			// TRS
			m_glTFObject->SetRot(glm::vec3(0.0f, 3.1415f * -1.0f, 0.0f)); // Sponza
			//m_glTFObject->SetScale(glm::vec3(500.0f)); // Spheres

			// MaterialInto
			std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
			createInfo->SetVertexShaderCode(m_VertexShader->GetData());
			createInfo->SetFragmentShaderCode(m_FragmentShader->GetData());

			// TextureSet
			std::shared_ptr<graphics::CTextureSet> TextureSet = std::make_shared<graphics::CTextureSet>();
			TextureSet->AddCubeMap(CubeTex);
			for(const auto& FrameTexture : m_FrameTextureList) { TextureSet->AddFrameTexture(FrameTexture); }
			TextureSet->AddIBLTexture(IBL_Diffuse_Tex, IBL_Specular_Tex, IBL_GGXLUT_Tex);

			// Import
			if (!gltf::CGLTFImporter::Import(pGraphicsAPI, m_glTFData->GetData(), m_glTFObject, createInfo, TextureSet, m_DepthVertex, m_DepthFragment)) return false;
		}

		//
		{
			// Material
			{
				std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
				createInfo->SetVertexShaderCode(m_MinimumVert->GetData());
				createInfo->SetFragmentShaderCode(m_TextureFrag->GetData());

				auto Mat = pGraphicsAPI->CreateMaterial(createInfo);

				auto UBO = graphics::CMaterialCreateInfo::CreateUniformBuffer({ graphics::SBindingLayout("UniformBufferObject", 0, false) });
				UBO->AddData("model", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UBO->AddData("view", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UBO->AddData("proj", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UBO->AddData("lightVPMat", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UBO->AddData("cameraPos", &glm::vec4(1.0f)[0], sizeof(glm::vec4), 0);
				UBO->AddData("useDirSampling", &glm::ivec1(1)[0], sizeof(glm::ivec1), 0);
				UBO->AddData("time", &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0);
				UBO->AddData("pad1", &glm::ivec1(0)[0], sizeof(glm::ivec1), 0);
				UBO->AddData("pad2", &glm::ivec1(0)[0], sizeof(glm::ivec1), 0);

				Mat->AddShaderBuffer(UBO);

				Mat->AddTextureBindingLayout({ "texImage", 1, 2, 0, graphics::ETextureUsage::TEXTURE_USAGE_2D });

				Mat->SetCullMode(graphics::ECullMode::CULL_FRONT);

				m_Background->AddMaterial(Mat);
			}

			// Mesh
			{
				std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

				std::shared_ptr<renderer::CRendererCreateInfo> createInfo = std::make_shared<renderer::CRendererCreateInfo>();
				graphics::CPresetPrimitive::CreateSphere(createInfo);

				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(createInfo, 0);
				Mesh->AddPrimitive(Primitive);

				m_Background->AddMesh(Mesh);
			}

			// Node
			{
				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, m_Background->GetMeshList(), m_Background->GetMaterialList());
				Node->SetScale(glm::vec3(500.0f));
				m_Background->AddNode(Node);
			}

			// TextureSet
			std::shared_ptr<graphics::CTextureSet> TextureSet = std::make_shared<graphics::CTextureSet>();
			TextureSet->Add2DTexture(IBL_Skybox_Tex);
			TextureSet->AddIBLTexture(IBL_Diffuse_Tex, IBL_Specular_Tex, IBL_GGXLUT_Tex);

			// Create
			if (!m_Background->Create(pGraphicsAPI, m_DepthVertex, m_DepthFragment, TextureSet)) return false;
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

		if (InputState->IsKeyDown(input::EKeyType::KEY_TYPE_1) && m_CoolTime <= 0.0f)
		{
			m_IsDrawSponza = (!m_IsDrawSponza);
			m_CoolTime = 0.5f;
		}
		else if (m_CoolTime > 0.0)
		{
			m_CoolTime -= DrawInfo->GetDeltaSecondsTime();
		}
		else
		{
			m_CoolTime = 0.0f;
		}

		if (m_IsDrawSponza && m_SponzaObject)
		{
			if (!m_SponzaObject->Update()) return false;
		}

		if (!m_IsDrawSponza && m_glTFObject)
		{
			if (!m_glTFObject->Update()) return false;
		}
		
		if (m_Background)
		{
			if (!m_Background->Update()) return false;
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

		if (m_IsDrawSponza && m_SponzaObject)
		{
			if (!m_SponzaObject->Draw(IsDepthPass, Camera, Projection, DrawInfo)) return false;
		}
		
		if (!m_IsDrawSponza && m_glTFObject)
		{
			if (!m_glTFObject->Draw(IsDepthPass, Camera, Projection, DrawInfo)) return false;
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