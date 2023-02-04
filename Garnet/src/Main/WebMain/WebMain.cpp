#include "../../App/CApp.h"
#include "../../App/CWebAppManager.h"
#include "../../Debug/Message/Console.h"
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

extern "C"
{

	webapp::CWebAppManager* g_WebApp = nullptr;

	void Release()
	{
		delete g_WebApp;
		g_WebApp = nullptr;
	}

	void RunLopp()
	{
		g_WebApp->RunLopp();

		if (!g_WebApp->IsRunLoop())
		{
			Release();
		}
	}

	int main()
	{
		g_WebApp = new webapp::CWebAppManager();

		g_WebApp->Initialize();
		emscripten_set_main_loop(RunLopp, 60, true);

		return 0;
	}

	EMSCRIPTEN_KEEPALIVE
	void OnKeyDown(char* key)
	{
		if (g_WebApp)
		{
			g_WebApp->OnKeyDown(std::string(key));
		}
	}
}
#endif // __EMSCRIPTEN__