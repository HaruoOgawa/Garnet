#include "CScriptApp.h"
#include "../../GraphicsAPI/CRendererCreateInfo.h"

namespace app
{
	CScriptApp::CScriptApp():
		m_TestRenderer(nullptr)
	{

	}

	CScriptApp::~CScriptApp()
	{

	}

	bool CScriptApp::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		// ひとまずトライアングルの描画テスト(後ほどリソース生成周りはもう少し整備する必要がある)
		m_TestRenderer = pGraphicsAPI->CreateRenderer();

		//
		renderer::CRendererCreateInfo createInfo;

		//
		if (!m_TestRenderer->Create(createInfo)) return false;

		return true;
	}

	bool CScriptApp::ProcessInput(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CScriptApp::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CScriptApp::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!pGraphicsAPI->BeginRender()) return false;

		// CScriptScene.cppにいろいろとAssets類を書く感じでもいいかも？
		if (!m_TestRenderer->Draw()) return false;

		if (!pGraphicsAPI->EndRender()) return false;

		return true;
	}
}