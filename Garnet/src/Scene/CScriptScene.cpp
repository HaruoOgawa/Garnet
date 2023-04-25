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
#ifdef __DAWN__
		m_VertexShader->ReadFile(ShaderPath + "vert.wgsl");
		m_FragmentShader->ReadFile(ShaderPath + "frag.wgsl");
#else
		m_VertexShader->ReadFile(ShaderPath + "shader_vert.spv");
		m_FragmentShader->ReadFile(ShaderPath + "shader_frag.spv");
#endif
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

		createInfo.SetVertexShaderCode(std::string(&m_VertexShader->GetData()[0], &m_VertexShader->GetData()[0] + m_VertexShader->GetData().size()));
		createInfo.SetFragmentShaderCode(std::string(&m_FragmentShader->GetData()[0], &m_FragmentShader->GetData()[0] + m_FragmentShader->GetData().size()));

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