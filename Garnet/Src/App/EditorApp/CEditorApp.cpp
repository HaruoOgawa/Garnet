#include "CEditorApp.h"

// CEditorApp はCMainAppで使うシーンファイルを作成するためのアプリ

namespace app
{
	CEditorApp::CEditorApp()
	{

	}

	CEditorApp::~CEditorApp()
	{

	}

	bool CEditorApp::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CEditorApp::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CEditorApp::ProcessInput(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CEditorApp::Resize(int Width, int Height)
	{
		return true;
	}

	bool CEditorApp::Update(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime)
	{
		return true;
	}

	bool CEditorApp::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}
}