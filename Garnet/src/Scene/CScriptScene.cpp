#include "CScriptScene.h"
#include "../Graphics/CMesh.h"
#include "../Graphics/CPrimitive.h"
#include "../GraphicsAPI/CRendererCreateInfo.h"
#include "../File/CFileReader.h"
#include "../Debug/Message/Console.h"

namespace scene
{
	CScriptScene::CScriptScene():
		m_TestMesh(nullptr),
		m_VertexShader(std::make_shared<file::CFileReader>()),
		m_FragmentShader(std::make_shared<file::CFileReader>()),
		m_IsLoaded(false)
	{
	}

	bool CScriptScene::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_TestMesh)
		{
			m_TestMesh->Release();
			m_TestMesh.reset();
			m_TestMesh = nullptr;
		}

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
		//
		std::string ShaderPath = "Resources\\Shaders\\";

		// Vertex Buffer
		std::vector<float> Pos = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f
		};

		std::vector<float> Color = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f
		};

		std::vector<float> UV = {
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		};

		std::vector<std::vector<float>> Vertices = {
			Pos, Color, UV
		};

		std::vector<uint16_t> Indices = {
			0, 1, 2, 2, 3, 0,
		};

		//
		renderer::CRendererCreateInfo createInfo;

		createInfo.SetVertexShaderCode(m_VertexShader->GetData());
		createInfo.SetFragmentShaderCode(m_FragmentShader->GetData());

		createInfo.SetVertices(Vertices);
		createInfo.SetIndices(Indices);
		createInfo.SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2 }));

		//
		m_TestMesh = std::make_shared<graphics::CMesh>();
		std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>();
		if (!Primitive->Create(pGraphicsAPI, createInfo)) return false;

		//
		m_TestMesh->AddPrimitive(Primitive);

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

		if (m_IsLoaded && m_TestMesh)
		{
			if (!m_TestMesh->Update(SecondsTime)) return false;

		}

		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_IsLoaded && m_TestMesh)
		{
			if (!m_TestMesh->Draw()) return false;

		}
		
		return true;
	}
}