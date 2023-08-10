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
			auto Material0 = pGraphicsAPI->CreateMaterial(createInfo);

			// UBO, TEXTURE
			{
				auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ graphics::SBindingLayout("UniformBufferObject", 0) });

				UniformBuffer->AddData("model", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("view", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("proj", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("lightVPMat", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);

				UniformBuffer->RecalculateBindingLayoutOffset();

				Material0->AddUniformBuffer(UniformBuffer);
			}

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

			// CreateŠÖ”‚ðŽÀs
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
			if (m_DepthVertex->IsLoaded() && m_DepthFragment->IsLoaded() && m_VertexShader->IsLoaded() && m_FragmentShader->IsLoaded())
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
}