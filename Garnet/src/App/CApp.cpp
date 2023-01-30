#include "CApp.h"
#include "../Debug/Message/Console.h"

namespace app
{
	CApp::CApp()
	{
		Console::Log("CApp::CApp\n");
	}

	CApp::~CApp()
	{
		Release();

		Console::Log("CApp::~CApp\n");
	}

	bool CApp::Release()
	{
		Console::Log("CApp::Release\n");
		return true;
	}

	bool CApp::Initialize()
	{
		Console::Log("CApp::Initialize\n");
		return true;
	}

	bool CApp::RunLopp()
	{
		Console::Log("CApp::RunLopp\n");
		return true;
	}
}