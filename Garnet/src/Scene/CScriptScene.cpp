#include "CScriptScene.h"
#include "../Object/C3DObject.h"
#include "../File/CFileReader.h"
#include "../Debug/Message/Console.h"

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
		// VERTEX
		std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(pGraphicsAPI, graphics::EPresetPrimitiveType::BOARD, 0);
		std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
		Mesh->AddPrimitive(Primitive);
		std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(Mesh);

		// RENDERER
		renderer::CRendererCreateInfo createInfo;
		createInfo.SetVertexShaderCode(m_VertexShader->GetData());
		createInfo.SetFragmentShaderCode(m_FragmentShader->GetData());
		std::shared_ptr<graphics::CMaterial> Material = std::make_shared <graphics::CMaterial>();
		if (!Material->Create(pGraphicsAPI, createInfo)) return false;

		// OBJECT
		m_TestObject = std::make_shared<object::C3DObject>();
		m_TestObject->AddNode(Node);
		m_TestObject->AddMaterial(Material);

		Console::Log("Render is loaded\n");

		return true;
	}

	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime)
	{
		if (!m_IsLoaded)
		{
			if (m_VertexShader->IsDone() && m_FragmentShader->IsDone())
			{
				if(!Load(pGraphicsAPI)) return false;
				m_IsLoaded = true;
			}
		}

		if (m_IsLoaded && m_TestObject)
		{
			if (!m_TestObject->Update(SecondsTime)) return false;

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