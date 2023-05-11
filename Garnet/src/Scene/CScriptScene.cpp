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
		Console::Log("___Loading Start____________________________________________________________________________________________________________________________________________________________\n");

		// OBJECT
		m_TestObject = std::make_shared<object::C3DObject>();

		{
			// MATERIAL
			graphics::CMaterialCreateInfo createInfo;
			createInfo.SetVertexShaderCode(m_VertexShader->GetData());
			createInfo.SetFragmentShaderCode(m_FragmentShader->GetData());
			auto Material = pGraphicsAPI->CreateMaterial();

			// UBO, TEXTURE
			{
				auto UniformBuffer = createInfo.CreateUniformBuffer({0, 1});

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

				Material->AddUniformBufferDescriptor(UniformBuffer->GetDescriptor());
				createInfo.AddBuffer(UniformBuffer);
			}
			
			// CREATE MATERIAL
			if (!Material->Create(pGraphicsAPI, createInfo)) return false;
			m_TestObject->AddMaterial(Material);
		}

		{
			// MESH
			std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(pGraphicsAPI, graphics::EPresetPrimitiveType::BOARD, 0, m_TestObject->GetMaterialList());
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
			Mesh->AddPrimitive(Primitive);

			// NODE
			std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(Mesh);
			m_TestObject->AddNode(Node);
		}
		
		Console::Log("___Render is loaded____________________________________________________________________________________________________________________________________________________________\n");

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