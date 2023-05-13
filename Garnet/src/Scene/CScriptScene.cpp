#include "CScriptScene.h"
#include "../Object/C3DObject.h"
#include "../File/CFileReader.h"
#include "../Debug/Message/Console.h"
#include <glm/glm.hpp>

namespace scene
{
	CScriptScene::CScriptScene():
		m_TestObject(nullptr),
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
		std::string ShaderPath = "Resources\\Shaders\\";

		m_VertexShader->ReadFile(ShaderPath + "sample_vert" + pGraphicsAPI->GetShaderExtension());
		m_FragmentShader->ReadFile(ShaderPath + "sample_frag" + pGraphicsAPI->GetShaderExtension());
		
		return true;
	}

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI)
	{
		// èâä˙âªèàóù
		// OBJECT
		m_TestObject = std::make_shared<object::C3DObject>();

		{
			// MATERIAL
			std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
			createInfo->SetVertexShaderCode(m_VertexShader->GetData());
			createInfo->SetFragmentShaderCode(m_FragmentShader->GetData());
			auto Material = pGraphicsAPI->CreateMaterial();

			// UBO, TEXTURE
			{
				auto UniformBuffer = createInfo->CreateUniformBuffer({0, 1});

				{
					glm::mat4 mat = glm::mat4(1.0f);
					UniformBuffer->AddData("model", &mat[0][0], sizeof(mat), 0);
				}

				{
					glm::mat4 mat = glm::mat4(1.0f);
					UniformBuffer->AddData("view", &mat[0][0], sizeof(mat), 0);
				}

				{
					glm::mat4 mat = glm::mat4(1.0f);
					UniformBuffer->AddData("proj", &mat[0][0], sizeof(mat), 0);
				}

				{
					glm::mat4 mat = glm::mat4(1.0f);
					UniformBuffer->AddData("mvp", &mat[0][0], sizeof(mat), 0);
				}

				{
					glm::vec4 val = glm::vec4(1.0f);
					UniformBuffer->AddData("MulColor", &val[0], sizeof(val), 1);
				}

				{
					glm::vec4 val = glm::vec4(0.0f);
					UniformBuffer->AddData("val0", &val[0], sizeof(val), 1);
				}

				{
					glm::vec4 val = glm::vec4(0.0f);
					UniformBuffer->AddData("val1", &val[0], sizeof(val), 1);
				}

				{
					glm::vec4 val = glm::vec4(0.0f);
					UniformBuffer->AddData("val2", &val[0], sizeof(val), 1);
				}

				UniformBuffer->RecalculateBindingLayoutOffset();

				Material->AddUniformBuffer(UniformBuffer);
			}
			
			// CREATE MATERIAL
			Material->SetCreateInfo(createInfo);
			m_TestObject->AddMaterial(Material);
		}

		{
			// MESH
			std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(0, m_TestObject->GetMaterialList(), std::make_shared<renderer::CRendererCreateInfo>(), graphics::EPresetPrimitiveType::BOARD);
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
			Mesh->AddPrimitive(Primitive);

			// NODE
			{
				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(Mesh);
				Node->SetPos(glm::vec3(0.5f, 0.0f, -2.0f));
				m_TestObject->AddNode(Node);
			}

			{
				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(Mesh);
				Node->SetPos(glm::vec3(-0.25f, 0.0f, -1.0f));
				Node->SetRot(glm::vec3(0.0f, 0.0f, 45.0f));
				m_TestObject->AddNode(Node);
			}
		}

		// Createä÷êîåQÇé¿çs
		if (!m_TestObject->Create(pGraphicsAPI)) return false;

		return true;
	}

	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection)
	{
		if (m_IsLoaded && m_TestObject)
		{
			if (!m_TestObject->Update(SecondsTime, Camera, Projection)) return false;

		}

		if (!m_IsLoaded)
		{
			if (m_VertexShader->IsDone() && m_FragmentShader->IsDone())
			{
				if(!Load(pGraphicsAPI)) return false;
				m_IsLoaded = true;
			}
		}

		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_IsLoaded && m_TestObject)
		{
			if (!m_TestObject->Draw()) return false;
		}
		
		return true;
	}
}