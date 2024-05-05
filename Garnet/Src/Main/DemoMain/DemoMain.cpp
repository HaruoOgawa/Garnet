#ifdef USE_WIN32_WindowAPI

#include "../../AppCore/CAppCore.h"
#include "../../Message/Console.h"

#include <Windows.h>

extern "C" {

	app::CAppCore* g_AppCore = nullptr;

	bool RunLoop()
	{
		if (!g_AppCore->RunLoop()) return false;

		return true;
	}

	void StartApp(HINSTANCE hInstance)
	{
		g_AppCore = new app::CAppCore();

		if (g_AppCore->Initialize(hInstance, 1920, 1080))
		{
			while (g_AppCore->IsRunLoop())
			{
				if (!RunLoop()) break;
			}
		}

		delete g_AppCore;
		g_AppCore = nullptr;
	}

	int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		StartApp(hInstance);

		return 0;
	}

}

#endif // USE_WIN32_WindowAPI