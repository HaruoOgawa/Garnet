#include "../../App/CDescAppManager.h"

descapp::CDescAppManager* g_DescApp = nullptr;

int WinMain()
{
	g_DescApp = new descapp::CDescAppManager();

	if (g_DescApp->Initialize())
	{
		g_DescApp->RunLopp();
	}

	delete g_DescApp;
	g_DescApp = nullptr;
	
	return 0;
}