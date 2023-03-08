#include <Windows.h>
#include "../../App/CDescAppManager.h"
#include "../../App/EAppType.h"

descapp::CDescAppManager* g_DescApp = nullptr;

#if defined(__DAWN__) && defined(__CMAKE__)
int main()
#else
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	g_DescApp = new descapp::CDescAppManager(app::EAppType::ScriptApp);

	if (g_DescApp->Initialize())
	{
		g_DescApp->RunLopp();
	}

	delete g_DescApp;
	g_DescApp = nullptr;
	
	return 0;
}