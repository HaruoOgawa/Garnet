#include "CDescAppManager.h"
#include "../Debug/Message/Console.h"

namespace descapp
{
	CDescAppManager::CDescAppManager()
	{
		Console::Log("CDescAppManager::CDescAppManager\n");
	}

	CDescAppManager::~CDescAppManager()
	{
		Release();

		Console::Log("CDescAppManager::~CDescAppManager\n");
	}

	bool CDescAppManager::Release()
	{
		Console::Log("CDescAppManager::Release\n");
		return true;
	}

	bool CDescAppManager::Initialize()
	{
		Console::Log("CDescAppManager::Initialize\n");
		return true;
	}

	bool CDescAppManager::RunLopp()
	{
		Console::Log("CDescAppManager::RunLopp\n");
		return true;
	}
}