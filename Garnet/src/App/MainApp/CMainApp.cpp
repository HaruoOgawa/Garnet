#include "CMainApp.h"

namespace app
{
	CMainApp::CMainApp()
	{

	}

	CMainApp::~CMainApp()
	{

	}

	bool CMainApp::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CMainApp::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CMainApp::ProcessInput(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CMainApp::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CMainApp::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}
}