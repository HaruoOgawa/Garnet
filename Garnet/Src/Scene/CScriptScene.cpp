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

		m_DepthVertex->ReadFile(ShaderPath + "depth" + pGraphicsAPI->GetVertexShaderExtension());
		m_DepthFragment->ReadFile(ShaderPath + "depth" + pGraphicsAPI->GetFragmentShaderExtension());

		m_VertexShader->ReadFile(ShaderPath + "sample" + pGraphicsAPI->GetVertexShaderExtension());
		m_FragmentShader->ReadFile(ShaderPath + "sample" + pGraphicsAPI->GetFragmentShaderExtension());

		m_glTFVert->ReadFile(ShaderPath + "gltfpbr" + pGraphicsAPI->GetVertexShaderExtension());

		{
			// これ以上のWebGPU Nativeのバグ対策で進まないのはだるいのでDescのWebGPU NativeではPCFをオフにした安全なものを使用する
#if defined(USE_WEBGPU) && !defined(__EMSCRIPTEN__)
			m_glTFFrag->ReadFile(ShaderPath + "gltfpbr_NativeSafe" + pGraphicsAPI->GetFragmentShaderExtension());
#else
			m_glTFFrag->ReadFile(ShaderPath + "gltfpbr" + pGraphicsAPI->GetFragmentShaderExtension());
#endif
		}

		m_ShadowDebugVertex->ReadFile(ShaderPath + "shadow_debug" + pGraphicsAPI->GetVertexShaderExtension());
		m_ShadowDebugFragment->ReadFile(ShaderPath + "shadow_debug" + pGraphicsAPI->GetFragmentShaderExtension());

		// Texture
		std::string TexturePath = "Resources\\Textures\\";

		m_Texture0->ReadFile(TexturePath + "brick.jpg");
		m_Texture1->ReadFile(TexturePath + "brick_norm.jpg");

		// GLTF
		std::string ModelPath = "Resources\\Models\\";

		m_Helmet_glTFData->ReadFile(ModelPath + "DamagedHelmet\\glTF-Binary\\DamagedHelmet.glb");

		m_Sphere_glTFData->ReadFile(ModelPath + "Box\\glTF-Binary\\Box.glb");
		//m_Sphere_glTFObj->SetScale(glm::vec3(500.0f, 500.0f, 500.0f));
		m_Sphere_glTFObj->SetPos(glm::vec3(-1.5f, -0.5f, -1.5f));

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

			// UBO, TEXTURE
			{
				auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ graphics::SBindingLayout("UniformBufferObject", 0) });

				UniformBuffer->AddData("model", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("view", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("proj", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("lightVPMat", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);

				//Material0->AddTextureBindingLayout({ 2, 3, -1, graphics::ETextureType::TEXTURE_2D });

				UniformBuffer->RecalculateBindingLayoutOffset();

				Material0->AddUniformBuffer(UniformBuffer);
			}

			{
				/*auto APITex0 = pGraphicsAPI->CreateTexture();
				if (!APITex0->Create(m_Texture0->GetData())) return false;

				auto APITex1 = pGraphicsAPI->CreateTexture();
				if (!APITex1->Create(m_Texture1->GetData())) return false;

				m_TestObject->AddTexture(APITex0);
				m_TestObject->AddTexture(APITex1);
				m_TestObject->AddTexture(m_FrameTextureList[0]); // ShadowMap*/
				//m_TestObject->AddCubeMap(CubeTexList[0]);
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
				//Node->SetPos(glm::vec3(0.0f, -1.0f, 0.0f));
				//Node->SetRot(glm::vec3(3.14f * (-0.5f), 0.0f, 0.0f));
				//Node->SetScale(glm::vec3(1.0f, 1.0f, 1.0f) * 10.0f);
				Node->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
				m_TestObject->AddNode(Node);
			}

			// Create関数を実行
			if (!m_TestObject->Create(pGraphicsAPI, m_DepthVertex, m_DepthFragment)) return false;
		}

		return true;
	}

	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_IsLoaded && m_TestObject)
		{
			if (!m_TestObject->Update()) return false;
		}

		if (!m_IsLoaded)
		{
			if (m_DepthVertex->IsLoaded() && m_DepthFragment->IsLoaded() && m_VertexShader->IsLoaded() && m_FragmentShader->IsLoaded() && m_Texture0->IsLoaded() && m_Texture1->IsLoaded()
				&& m_Sphere_glTFData->IsLoaded() && m_Helmet_glTFData->IsLoaded() && m_glTFVert->IsLoaded() && m_glTFFrag->IsLoaded() && m_ShadowDebugVertex->IsLoaded() && m_ShadowDebugFragment->IsLoaded()
				&& m_Cube0->IsLoaded() && m_Cube1->IsLoaded() && m_Cube2->IsLoaded() && m_Cube3->IsLoaded() && m_Cube4->IsLoaded() && m_Cube5->IsLoaded()
				)
			{
				if (!Load(pGraphicsAPI)) return false;
				m_IsLoaded = true;
			}
		}

		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (m_IsLoaded && m_TestObject)
		{
			if (!m_TestObject->Draw(IsDepthPass, SecondsTime, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	// Tex of FrameBuffer
	void CScriptScene::SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
	}
}