#include "CScriptScene.h"
#include "../Graphics/CMesh.h"
#include "../Graphics/CPrimitive.h"
#include "../GraphicsAPI/CRendererCreateInfo.h"
#include "../File/CFile.h"
#include "../Graphics/SVertex.h"

namespace scene
{
	CScriptScene::CScriptScene():
		m_TestMesh(nullptr)
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

		return true;
	}

	bool CScriptScene::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		//
		std::string ProjDir = "WebRelease\\sample\\";
		std::string Resources = "Resources\\Shaders\\";

		// Vertex Buffer
		std::vector<renderer::SVertex> Vertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f},   {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f},	 {1.0f, 1.0f}},
		};

		std::vector<uint16_t> Indices = {
			0, 1, 2, 2, 3, 0,
		};

		//
		renderer::CRendererCreateInfo createInfo;
		createInfo.SetVertexShaderCode(file::CFile::ReadFile(ProjDir + Resources + "vert.spv"));
		createInfo.SetFragmentShaderCode(file::CFile::ReadFile(ProjDir + Resources + "frag.spv"));
		createInfo.SetVertices(Vertices);
		createInfo.SetIndices(Indices);

		//
		m_TestMesh = std::make_shared<graphics::CMesh>();
		std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>();
		if (!Primitive->Create(pGraphicsAPI, createInfo)) return false;

		//
		m_TestMesh->AddPrimitive(Primitive);

		return true;
	}
	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_TestMesh->Update()) return false;

		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_TestMesh->Draw()) return false;

		return true;
	}
}