#include "CScriptScene.h"
#include "../Object/C3DObject.h"
#include "../File/CFileReader.h"
#include "../Debug/Message/Console.h"
#include "../GLTF/CGLTFImporter.h"
#include <glm/glm.hpp>
#include "../Graphics/CDrawInfo.h"
#include "../Camera/CCamera.h"

namespace scene
{
	CScriptScene::CScriptScene():
		m_TestObject(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_DepthVertex(std::make_shared<file::CFileReader>()),
		m_DepthFragment(std::make_shared<file::CFileReader>()),
		
		m_VertexShader(std::make_shared<file::CFileReader>()),
		m_FragmentShader(std::make_shared<file::CFileReader>()),
		m_Texture0(std::make_shared<file::CFileReader>()),
		m_Texture1(std::make_shared<file::CFileReader>()),

		m_Sphere_glTFObj(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_Sphere_glTFData(std::make_shared<file::CFileReader>()),
		m_Helmet_glTFObj(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_Helmet_glTFData(std::make_shared<file::CFileReader>()),
		m_glTFVert(std::make_shared<file::CFileReader>()),
		m_glTFFrag(std::make_shared<file::CFileReader>()),

		m_Cube0(std::make_shared<file::CFileReader>()),
		m_Cube1(std::make_shared<file::CFileReader>()),
		m_Cube2(std::make_shared<file::CFileReader>()),
		m_Cube3(std::make_shared<file::CFileReader>()),
		m_Cube4(std::make_shared<file::CFileReader>()),
		m_Cube5(std::make_shared<file::CFileReader>()),

		m_DepthDebugObj(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_ShadowDebugVertex(std::make_shared<file::CFileReader>()),
		m_ShadowDebugFragment(std::make_shared<file::CFileReader>()),

		m_IsLoaded(false)
	{
	}

	bool CScriptScene::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		m_IsLoaded = false;

		return true;
	}

	bool CScriptScene::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		// Shader
		std::string ShaderPath = "Resources\\Shaders\\";
		
		m_DepthVertex->ReadFile(ShaderPath + "depth_vert" + pGraphicsAPI->GetShaderExtension());
		m_DepthFragment->ReadFile(ShaderPath + "depth_frag" + pGraphicsAPI->GetShaderExtension());
		
		m_VertexShader->ReadFile(ShaderPath + "sample_vert" + pGraphicsAPI->GetShaderExtension());
		m_FragmentShader->ReadFile(ShaderPath + "sample_frag" + pGraphicsAPI->GetShaderExtension());
		
		m_glTFVert->ReadFile(ShaderPath + "gltfpbr_vert" + pGraphicsAPI->GetShaderExtension());
		m_glTFFrag->ReadFile(ShaderPath + "gltfpbr_frag" + pGraphicsAPI->GetShaderExtension());
		
		m_ShadowDebugVertex->ReadFile(ShaderPath + "shadow_debug_vert" + pGraphicsAPI->GetShaderExtension());
		m_ShadowDebugFragment->ReadFile(ShaderPath + "shadow_debug_frag" + pGraphicsAPI->GetShaderExtension());
		
		// Texture
		std::string TexturePath = "Resources\\Textures\\";
		
		m_Texture0->ReadFile(TexturePath + "brick.jpg");
		m_Texture1->ReadFile(TexturePath + "brick_norm.jpg");

		// GLTF
		std::string ModelPath = "Resources\\Models\\";

		m_Helmet_glTFData->ReadFile(ModelPath + "DamagedHelmet\\glTF-Binary\\DamagedHelmet.glb");

		m_Sphere_glTFData->ReadFile(ModelPath + "Box\\glTF-Binary\\Box.glb");
		//m_Sphere_glTFObj->SetScale(glm::vec3(500.0f, 500.0f, 500.0f));
		m_Sphere_glTFObj->SetPos(glm::vec3(-1.5f, 0.0f, -1.5f));

		// Cubemap
		m_Cube0->ReadFile("Resources\\Cubemaps\\environment\\environment_back_0.jpg");
		m_Cube1->ReadFile("Resources\\Cubemaps\\environment\\environment_bottom_0.jpg");
		m_Cube2->ReadFile("Resources\\Cubemaps\\environment\\environment_front_0.jpg");
		m_Cube3->ReadFile("Resources\\Cubemaps\\environment\\environment_left_0.jpg");
		m_Cube4->ReadFile("Resources\\Cubemaps\\environment\\environment_right_0.jpg");
		m_Cube5->ReadFile("Resources\\Cubemaps\\environment\\environment_top_0.jpg");

		return true;
	}

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI)
	{
		// Cubemap
		std::vector<std::shared_ptr<graphics::CTexture>> CubeTexList;
		{
			std::vector<std::vector<unsigned char>> CubeDataList;
			CubeDataList.push_back(m_Cube0->GetData());
			CubeDataList.push_back(m_Cube1->GetData());
			CubeDataList.push_back(m_Cube2->GetData());
			CubeDataList.push_back(m_Cube3->GetData());
			CubeDataList.push_back(m_Cube4->GetData());
			CubeDataList.push_back(m_Cube5->GetData());

			auto CubeTex0 = pGraphicsAPI->CreateTexture(true);
			if (!CubeTex0->Create(CubeDataList)) return false;

			CubeTexList.push_back(CubeTex0);
		}

		// TestObj
		{
			// MATERIAL
			std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
			createInfo->SetVertexShaderCode(m_glTFVert->GetData());
			createInfo->SetFragmentShaderCode(m_glTFFrag->GetData());
			auto Material0 = pGraphicsAPI->CreateMaterial();

			// UBO, TEXTURE
			{
				auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({0});

				UniformBuffer->AddData("model", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("view", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("proj", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("lightVPMat", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("lightDir", &glm::vec4(0.0f)[0], sizeof(float) * 4, 0);
				UniformBuffer->AddData("lightColor", &glm::vec4(0.0f)[0], sizeof(float) * 4, 0);
				UniformBuffer->AddData("cameraPos", &glm::vec4(0.0f)[0], sizeof(float) * 4, 0);
				UniformBuffer->AddData("baseColorFactor", &glm::vec4(1.0f)[0], sizeof(float) * 4, 0);
				UniformBuffer->AddData("emissiveFactor", &glm::vec4(0.0f)[0], sizeof(float) * 4, 0);

				UniformBuffer->AddData("time", &glm::vec1(0.0f)[0], sizeof(float), 0);
				UniformBuffer->AddData("metallicFactor", &glm::vec1(1.0f)[0], sizeof(float), 0);
				UniformBuffer->AddData("roughnessFactor", &glm::vec1(0.1f)[0], sizeof(float), 0);
				UniformBuffer->AddData("normalMapScale", &glm::vec1(1.0f)[0], sizeof(float), 0);

				UniformBuffer->AddData("occlusionStrength", &glm::vec1(0.0f)[0], sizeof(float), 0);
				UniformBuffer->AddData("mipCount", &glm::vec1(CubeTexList[0]->GetMipCount())[0], sizeof(float), 0);
				UniformBuffer->AddData("s_pad1", &glm::vec1(0.0f)[0], sizeof(float), 0);
				UniformBuffer->AddData("s_pad2", &glm::vec1(0.0f)[0], sizeof(float), 0);

				UniformBuffer->AddData("useBaseColorTexture", &glm::uvec1(1)[0], sizeof(int), 0);
				UniformBuffer->AddData("useMetallicRoughnessTexture", &glm::uvec1(0)[0], sizeof(int), 0);
				UniformBuffer->AddData("useEmissiveTexture", &glm::uvec1(0)[0], sizeof(int), 0);
				UniformBuffer->AddData("useNormalTexture", &glm::uvec1(1)[0], sizeof(int), 0);
				
				UniformBuffer->AddData("useOcclusionTexture", &glm::uvec1(0)[0], sizeof(int), 0);
				UniformBuffer->AddData("t_pad_0", &glm::uvec1(0)[0], sizeof(int), 0);
				UniformBuffer->AddData("t_pad_1", &glm::uvec1(0)[0], sizeof(int), 0);
				UniformBuffer->AddData("t_pad_2", &glm::uvec1(0)[0], sizeof(int), 0);
				
				Material0->AddTextureBindingLayout({ 1, 2, 0, graphics::ETextureType::TEXTURE_2D });
				Material0->AddTextureBindingLayout({ 3, 4, -1, graphics::ETextureType::TEXTURE_2D }); // TextureIndex -1 は EmptyTextureである
				Material0->AddTextureBindingLayout({ 5, 6, -1, graphics::ETextureType::TEXTURE_2D }); // TextureIndex -1 は EmptyTextureである
				Material0->AddTextureBindingLayout({ 7, 8, 1, graphics::ETextureType::TEXTURE_2D }); // TextureIndex -1 は EmptyTextureである
				Material0->AddTextureBindingLayout({ 9, 10, -1, graphics::ETextureType::TEXTURE_2D }); // TextureIndex -1 は EmptyTextureである
				Material0->AddTextureBindingLayout({ 11, 12, 0, graphics::ETextureType::TEXTURE_CUBE });
				Material0->AddTextureBindingLayout({ 13, 14, 2, graphics::ETextureType::TEXTURE_2D }); // ひとまず1にShadowMapを入れている

				UniformBuffer->RecalculateBindingLayoutOffset();

				Material0->AddUniformBuffer(UniformBuffer);
			}

			{
				auto APITex0 = pGraphicsAPI->CreateTexture();
				if(!APITex0->Create(m_Texture0->GetData())) return false;
				
				auto APITex1 = pGraphicsAPI->CreateTexture();
				if(!APITex1->Create(m_Texture1->GetData())) return false;

				m_TestObject->AddTexture(APITex0);
				m_TestObject->AddTexture(APITex1);
				m_TestObject->AddTexture(m_FrameTextureList[0]); // ShadowMap
				m_TestObject->AddCubeMap(CubeTexList[0]);
			}
			
			// CREATE MATERIAL
			Material0->SetCreateInfo(createInfo);
			
			m_TestObject->AddMaterial(Material0);

			// MESH
			std::shared_ptr<graphics::CMesh> Mesh0 = std::make_shared<graphics::CMesh>();

			{
				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(nullptr, 0, graphics::EPresetPrimitiveType::BOARD);
				Mesh0->AddPrimitive(Primitive);
				m_TestObject->AddMesh(Mesh0);
			}

			// NODE
			{
				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, m_TestObject->GetMeshList(), m_TestObject->GetMaterialList());
				Node->SetMeshIndex(0);
				Node->SetPos(glm::vec3(0.0f, -1.0f, 0.0f));
				Node->SetRot(glm::vec3(3.14f * (-0.5f), 0.0f, 0.0f));
				Node->SetScale(glm::vec3(1.0f, 1.0f, 1.0f) * 10.0f);
				m_TestObject->AddNode(Node);
			}

			// Create関数を実行
			if (!m_TestObject->Create(pGraphicsAPI, m_DepthVertex, m_DepthFragment)) return false;
		}

		// DepthDebug
		{
			std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
			createInfo->SetVertexShaderCode(m_ShadowDebugVertex->GetData());
			createInfo->SetFragmentShaderCode(m_ShadowDebugFragment->GetData());
			
			auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ 0 });
			UniformBuffer->AddData("model", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
			UniformBuffer->AddData("view", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
			UniformBuffer->AddData("proj", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
			UniformBuffer->AddData("lightVPMat", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);

			auto Material = pGraphicsAPI->CreateMaterial();
			Material->SetCreateInfo(createInfo);
			Material->AddUniformBuffer(UniformBuffer);
			Material->AddTextureBindingLayout({ 1, 2, 0, graphics::ETextureType::TEXTURE_2D });

			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
			std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(nullptr, 0, graphics::EPresetPrimitiveType::BOARD);
			Mesh->AddPrimitive(Primitive);

			m_DepthDebugObj->AddTexture(m_FrameTextureList[0]);
			m_DepthDebugObj->AddMaterial(Material);
			m_DepthDebugObj->AddMesh(Mesh);

			std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, m_DepthDebugObj->GetMeshList(), m_DepthDebugObj->GetMaterialList());
			Node->SetMeshIndex(0);
			Node->SetPos(glm::vec3(0.0f, 1.0f, -3.0f));
			Node->SetRot(glm::vec3(0.0f, 0.0f, 0.0f));
			Node->SetScale(glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f);

			m_DepthDebugObj->AddNode(Node);

			if (!m_DepthDebugObj->Create(pGraphicsAPI, m_DepthVertex, m_DepthFragment)) return false;
		}

		// テストのglTFをインポート
		{
			std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
			createInfo->SetVertexShaderCode(m_glTFVert->GetData());
			createInfo->SetFragmentShaderCode(m_glTFFrag->GetData());

			if (!gltf::CGLTFImporter::Import(pGraphicsAPI, m_Sphere_glTFData->GetData(), m_Sphere_glTFObj, createInfo, CubeTexList, m_FrameTextureList, m_DepthVertex, m_DepthFragment)) return false;
			if (!gltf::CGLTFImporter::Import(pGraphicsAPI, m_Helmet_glTFData->GetData(), m_Helmet_glTFObj, createInfo, CubeTexList, m_FrameTextureList, m_DepthVertex, m_DepthFragment)) return false;
		}

		return true;
	}

	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_IsLoaded && m_TestObject)
		{
			if (!m_TestObject->Update()) return false;
		}
		
		if (m_IsLoaded && m_DepthDebugObj)
		{
			if (!m_DepthDebugObj->Update()) return false;
		}
		
		if (m_IsLoaded && m_Sphere_glTFObj)
		{
			if (!m_Sphere_glTFObj->Update()) return false;
		}
		
		if (m_IsLoaded && m_Helmet_glTFObj)
		{
			if (!m_Helmet_glTFObj->Update()) return false;
		}

		if (!m_IsLoaded)
		{
			if (m_DepthVertex->IsLoaded() && m_DepthFragment->IsLoaded() &&m_VertexShader->IsLoaded() && m_FragmentShader->IsLoaded() && m_Texture0->IsLoaded() && m_Texture1->IsLoaded()
				&& m_Sphere_glTFData->IsLoaded() && m_Helmet_glTFData->IsLoaded() && m_glTFVert->IsLoaded() && m_glTFFrag->IsLoaded()&& m_ShadowDebugVertex->IsLoaded() && m_ShadowDebugFragment->IsLoaded()
				&& m_Cube0->IsLoaded() && m_Cube1->IsLoaded() && m_Cube2->IsLoaded() && m_Cube3->IsLoaded() && m_Cube4->IsLoaded() && m_Cube5->IsLoaded() 
			)
			{
				if(!Load(pGraphicsAPI)) return false;
				m_IsLoaded = true;
			}
		}

		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (m_IsLoaded && m_Sphere_glTFObj)
		{
			if (!m_Sphere_glTFObj->Draw(IsDepthPass, SecondsTime, Camera, Projection, DrawInfo)) return false;
		}
		
		if (m_IsLoaded && m_Helmet_glTFObj)
		{
			if (!m_Helmet_glTFObj->Draw(IsDepthPass, SecondsTime, Camera, Projection, DrawInfo)) return false;
		}
		
		if (m_IsLoaded && m_TestObject)
		{
			if (!m_TestObject->Draw(IsDepthPass, SecondsTime, Camera, Projection, DrawInfo)) return false;
		}
		
		return true;
	}
	
	bool CScriptScene::DrawDebugObj(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (m_IsLoaded && m_DepthDebugObj)
		{
			m_DepthDebugObj->SetPos(DrawInfo->GetLightCamera()->GetPos());
			if (!m_DepthDebugObj->Draw(false, SecondsTime, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	// Tex of FrameBuffer
	void CScriptScene::SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
		m_FrameTextureList.push_back(FrameTexture);
	}
}