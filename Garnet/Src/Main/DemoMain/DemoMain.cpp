#ifdef USE_OPENGL

#include "../../App/CDemoAppManager.h"
#include "../../App/EAppType.h"
#include "../../Debug/Message/Console.h"

#include <Windows.h>

extern "C" {

	app::CDemoAppManager* g_DemoApp = nullptr;

	bool RunLopp()
	{
		if (!g_DemoApp->RunLopp()) return false;

		return true;
	}

	void StartApp()
	{
		g_DemoApp = new app::CDemoAppManager(app::EAppType::ScriptApp);

		if (g_DemoApp->Initialize())
		{
			while (g_DemoApp->IsRunLoop())
			{
				if (!RunLopp()) break;
			}
		}

		delete g_DemoApp;
		g_DemoApp = nullptr;
	}

	int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		StartApp();

		return 0;
	}

}

#endif // USE_OPENGL