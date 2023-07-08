#include "CScriptScene.h"
#include "../Object/C3DObject.h"
#include "../File/CFileReader.h"
#include "../Debug/Message/Console.h"
#include "../GLTF/CGLTFImporter.h"
#include <glm/glm.hpp>

namespace scene
{
	CScriptScene::CScriptScene():
		m_TestObject(std::make_shared<object::C3DObject>("Test")),
		m_VertexShader(std::make_shared<file::CFileReader>()),
		m_FragmentShader(std::make_shared<file::CFileReader>()),
		m_Texture0(std::make_shared<file::CFileReader>()),
		m_Texture1(std::make_shared<file::CFileReader>()),

		m_Sphere_glTFObj(std::make_shared<object::C3DObject>()),
		m_Sphere_glTFData(std::make_shared<file::CFileReader>()),
		m_Helmet_glTFObj(std::make_shared<object::C3DObject>()),
		m_Helmet_glTFData(std::make_shared<file::CFileReader>()),
		m_glTFVert(std::make_shared<file::CFileReader>()),
		m_glTFFrag(std::make_shared<file::CFileReader>()),

		m_Cube0(std::make_shared<file::CFileReader>()),
		m_Cube1(std::make_shared<file::CFileReader>()),
		m_Cube2(std::make_shared<file::CFileReader>()),
		m_Cube3(std::make_shared<file::CFileReader>()),
		m_Cube4(std::make_shared<file::CFileReader>()),
		m_Cube5(std::make_shared<file::CFileReader>()),

		m_DepthDebugObj(std::make_shared<object::C3DObject>()),
		m_ShadowVertex(std::make_shared<file::CFileReader>()),
		m_ShadowFragment(std::make_shared<file::CFileReader>()),

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
		
		m_VertexShader->ReadFile(ShaderPath + "sample_vert" + pGraphicsAPI->GetShaderExtension());
		m_FragmentShader->ReadFile(ShaderPath + "sample_frag" + pGraphicsAPI->GetShaderExtension());
		
		m_glTFVert->ReadFile(ShaderPath + "gltfpbr_vert" + pGraphicsAPI->GetShaderExtension());
		m_glTFFrag->ReadFile(ShaderPath + "gltfpbr_frag" + pGraphicsAPI->GetShaderExtension());
		
		m_ShadowVertex->ReadFile(ShaderPath + "shadow_vert" + pGraphicsAPI->GetShaderExtension());
		m_ShadowFragment->ReadFile(ShaderPath + "shadow_frag" + pGraphicsAPI->GetShaderExtension());
		
		// Texture
		std::string TexturePath = "Resources\\Textures\\";
		
		m_Texture0->ReadFile(TexturePath + "perlinnoise.png");
		m_Texture1->ReadFile(TexturePath + "UVTile.jpg");

		// GLTF
		std::string ModelPath = "Resources\\Models\\";

		m_Helmet_glTFData->ReadFile(ModelPath + "DamagedHelmet\\glTF-Binary\\DamagedHelmet.glb");

		m_Sphere_glTFData->ReadFile(ModelPath + "MetalRoughSpheresNoTextures\\glTF-Binary\\MetalRoughSpheresNoTextures.glb");
		m_Sphere_glTFObj->SetScale(glm::vec3(500.0f, 500.0f, 500.0f));
		m_Sphere_glTFObj->SetPos(glm::vec3(-1.5f, -1.5f, -1.5f));

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
		// TestObj
		{
			// MATERIAL
			std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
			createInfo->SetVertexShaderCode(m_VertexShader->GetData());
			createInfo->SetFragmentShaderCode(m_FragmentShader->GetData());
			auto Material0 = pGraphicsAPI->CreateMaterial();
			auto Material1 = pGraphicsAPI->CreateMaterial();

			// UBO, TEXTURE
			{
				auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({0, 1});

				UniformBuffer->AddData("model", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("view", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("proj", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("mvp", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("MulColor", &glm::vec4(1.0f)[0], sizeof(glm::vec4), 1);
				UniformBuffer->AddData("val0", &glm::vec4(0.0f)[0], sizeof(glm::vec4), 1);
				UniformBuffer->AddData("val1", &glm::vec4(0.0f)[0], sizeof(glm::vec4), 1);
				UniformBuffer->AddData("val2", &glm::vec4(0.0f)[0], sizeof(glm::vec4), 1);

				UniformBuffer->RecalculateBindingLayoutOffset();

				Material0->AddUniformBuffer(UniformBuffer);
				Material1->AddUniformBuffer(UniformBuffer);
			}

			{
				auto APITex0 = pGraphicsAPI->CreateTexture();
				if(!APITex0->Create(m_Texture0->GetData())) return false;

				auto APITex1 = pGraphicsAPI->CreateTexture();
				if (!APITex1->Create(m_Texture1->GetData())) return false;

				Material0->AddTextureBindingLayout({ 2, 3, 0, graphics::ETextureType::TEXTURE_2D });
				Material1->AddTextureBindingLayout({ 2, 3, 1, graphics::ETextureType::TEXTURE_2D });

				m_TestObject->AddTexture(APITex0);
				m_TestObject->AddTexture(APITex1);
			}
			
			// CREATE MATERIAL
			Material0->SetCreateInfo(createInfo);
			Material1->SetCreateInfo(createInfo);
			
			m_TestObject->AddMaterial(Material0);
			m_TestObject->AddMaterial(Material1);

			// MESH
			std::shared_ptr<graphics::CMesh> Mesh0 = std::make_shared<graphics::CMesh>();
			std::shared_ptr<graphics::CMesh> Mesh1 = std::make_shared<graphics::CMesh>();
			

			{
				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(nullptr, 0, graphics::EPresetPrimitiveType::BOARD);
				Mesh0->AddPrimitive(Primitive);
				m_TestObject->AddMesh(Mesh0);
			}

			{
				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(nullptr, 1, graphics::EPresetPrimitiveType::BOARD);
				Mesh1->AddPrimitive(Primitive);
				m_TestObject->AddMesh(Mesh1);
			}

			// NODE
			{
				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, m_TestObject->GetMeshList(), m_TestObject->GetMaterialList());
				Node->SetMeshIndex(0);
				Node->SetPos(glm::vec3(0.0f, -1.0f, 0.0f));
				Node->SetRot(glm::vec3(0.0f, 0.0f, 45.0f));
				Node->SetScale(glm::vec3(1.0f, 0.1f, 1.0f) * 5.0f);
				m_TestObject->AddNode(Node);
			}

			{
				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(1, m_TestObject->GetMeshList(), m_TestObject->GetMaterialList());
				Node->SetMeshIndex(1);
				Node->SetPos(glm::vec3(-1.25f, 0.0f, -5.0f));
				Node->SetRot(glm::vec3(0.0f, 45.0f, 45.0f));
				Node->SetScale(glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f);
				m_TestObject->AddNode(Node);
			}

			{
				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, m_TestObject->GetMeshList(), m_TestObject->GetMaterialList());
				Node->SetMeshIndex(0);
				Node->SetPos(glm::vec3(1.5f, 0.0f, -5.0f));
				Node->SetRot(glm::vec3(0.0f, -45.0f, 0.0f));
				Node->SetScale(glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f);
				m_TestObject->AddNode(Node);
			}

			// Create関数を実行
			if (!m_TestObject->Create(pGraphicsAPI)) return false;
		}

		// DepthDebug
		{
			std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
			createInfo->SetVertexShaderCode(m_ShadowVertex->GetData());
			createInfo->SetFragmentShaderCode(m_ShadowFragment->GetData());
			
			auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ 0 });
			UniformBuffer->AddData("model", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
			UniformBuffer->AddData("view", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
			UniformBuffer->AddData("proj", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);

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
			Node->SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
			Node->SetRot(glm::vec3(0.0f, 0.0f, 0.0f));
			Node->SetScale(glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f);

			m_DepthDebugObj->AddNode(Node);

			if (!m_DepthDebugObj->Create(pGraphicsAPI)) return false;
		}

		// Cubemap
		std::vector<std::vector<unsigned char>> CubeDataList;
		CubeDataList.push_back(m_Cube0->GetData());
		CubeDataList.push_back(m_Cube1->GetData());
		CubeDataList.push_back(m_Cube2->GetData());
		CubeDataList.push_back(m_Cube3->GetData());
		CubeDataList.push_back(m_Cube4->GetData());
		CubeDataList.push_back(m_Cube5->GetData());

		auto CubeTex0 = pGraphicsAPI->CreateTexture(true);
		if (!CubeTex0->Create(CubeDataList)) return false;

		std::vector<std::shared_ptr<graphics::CTexture>> CubeTexList;
		CubeTexList.push_back(CubeTex0);

		// テストのglTFをインポート
		{
			std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
			createInfo->SetVertexShaderCode(m_glTFVert->GetData());
			createInfo->SetFragmentShaderCode(m_glTFFrag->GetData());

			if (!gltf::CGLTFImporter::Import(pGraphicsAPI, m_Sphere_glTFData->GetData(), m_Sphere_glTFObj, createInfo, CubeTexList)) return false;
			if (!gltf::CGLTFImporter::Import(pGraphicsAPI, m_Helmet_glTFData->GetData(), m_Helmet_glTFObj, createInfo, CubeTexList)) return false;
		}

		return true;
	}

	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (m_IsLoaded && m_TestObject)
		{
			m_TestObject->GetNodeList()[0]->SetRot(glm::vec3(SecondsTime));

			if (!m_TestObject->Update(SecondsTime, Camera, Projection, DrawInfo)) return false;
		}
		
		if (m_IsLoaded && m_DepthDebugObj)
		{
			if (!m_DepthDebugObj->Update(SecondsTime, Camera, Projection, DrawInfo)) return false;
		}
		
		if (m_IsLoaded && m_Sphere_glTFObj)
		{
			if (!m_Sphere_glTFObj->Update(SecondsTime, Camera, Projection, DrawInfo)) return false;
		}
		
		if (m_IsLoaded && m_Helmet_glTFObj)
		{
			if (!m_Helmet_glTFObj->Update(SecondsTime, Camera, Projection, DrawInfo)) return false;
		}

		if (!m_IsLoaded)
		{
			if (m_VertexShader->IsLoaded() && m_FragmentShader->IsLoaded() && m_Texture0->IsLoaded() && m_Texture1->IsLoaded() && m_Sphere_glTFData->IsLoaded() && m_Helmet_glTFData->IsLoaded()
				&& m_glTFVert->IsLoaded() && m_glTFFrag->IsLoaded()&& m_ShadowVertex->IsLoaded() && m_ShadowFragment->IsLoaded()
				&& m_Cube0->IsLoaded() && m_Cube1->IsLoaded() && m_Cube2->IsLoaded() && m_Cube3->IsLoaded() && m_Cube4->IsLoaded() && m_Cube5->IsLoaded() 
			)
			{
				if(!Load(pGraphicsAPI)) return false;
				m_IsLoaded = true;
			}
		}

		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_IsLoaded && m_Sphere_glTFObj)
		{
			if (!m_Sphere_glTFObj->Draw()) return false;
		}
		
		if (m_IsLoaded && m_Helmet_glTFObj)
		{
			if (!m_Helmet_glTFObj->Draw()) return false;
		}
		
		if (m_IsLoaded && m_DepthDebugObj)
		{
			if (!m_DepthDebugObj->Draw()) return false;
		}
		
		return true;
	}
	
	bool CScriptScene::DrawTest(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_IsLoaded && m_TestObject)
		{
			if (!m_TestObject->Draw()) return false;
		}
		
		return true;
	}

	// Tex of FrameBuffer
	void CScriptScene::SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
		m_FrameTextureList.push_back(FrameTexture);
	}
}