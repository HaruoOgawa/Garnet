#include "CScriptApp.h"

// ひとまず仮でテスト用にインクルードしている
#include "../../Graphics/CMesh.h"
#include "../../Graphics/CPrimitive.h"
#include "../../GraphicsAPI/CRendererCreateInfo.h"
#include "../../File/CFile.h"

namespace app
{
	CScriptApp::CScriptApp():
		m_TestMesh(nullptr)
	{

	}

	CScriptApp::~CScriptApp()
	{
	}

	bool CScriptApp::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_TestMesh)
		{
			m_TestMesh->Release();
			m_TestMesh.reset();
			m_TestMesh = nullptr;
		}
		
		return true;
	}

	bool CScriptApp::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		//
		std::string ProjDir = "WebRelease\\sample\\";
		std::string Resources = "Resources\\Shaders\\";

		//
		renderer::CRendererCreateInfo createInfo;
		createInfo.SetVertexShaderCode(file::CFile::ReadFile(ProjDir + Resources + "vert.spv"));
		createInfo.SetFragmentShaderCode(file::CFile::ReadFile(ProjDir + Resources + "frag.spv"));

		//
		m_TestMesh = std::make_shared<graphics::CMesh>();
		std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>();
		if (!Primitive->Create(pGraphicsAPI, createInfo)) return false;

		//
		m_TestMesh->AddPrimitive(Primitive);

		return true;
	}

	bool CScriptApp::ProcessInput(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CScriptApp::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_TestMesh->Update()) return false;

		return true;
	}

	bool CScriptApp::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!pGraphicsAPI->BeginRender()) return false;

		// CScriptScene.cppにいろいろとAssets類を書く感じでもいいかも？
		if (!pGraphicsAPI->IsWaitting())
		{
			if (!m_TestMesh->Draw()) return false;
		}

		if (!pGraphicsAPI->EndRender()) return false;

		return true;
	}
}