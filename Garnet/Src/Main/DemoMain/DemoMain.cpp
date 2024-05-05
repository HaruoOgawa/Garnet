#ifdef USE_WIN32_WindowAPI

#include "../../WindowAPI/CWin32WindowAPI.h"
#include "../../Message/Console.h"

#include <Windows.h>

extern "C" {

	app::CWin32WindowAPI* g_DemoApp = nullptr;

	bool RunLopp()
	{
		if (!g_DemoApp->RunLopp()) return false;

		return true;
	}

	void StartApp(HINSTANCE hInstance)
	{
		g_DemoApp = new app::CWin32WindowAPI();

		if (g_DemoApp->Initialize(hInstance))
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
		StartApp(hInstance);

		return 0;
	}

}

#endif // USE_WIN32_WindowAPI