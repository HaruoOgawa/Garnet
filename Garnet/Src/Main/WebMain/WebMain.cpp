#include "../../App/CWebAppManager.h"
#include "../../Debug/Message/Console.h"
#include "../../App/EAppType.h"

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
		if (!g_WebApp->RunLoop() || !g_WebApp->IsRunLoop())
		{
			Release();
		}
	}

	EMSCRIPTEN_KEEPALIVE
	void StartApp(int Width, int Height)
	{
		g_WebApp = new webapp::CWebAppManager(app::EAppType::ScriptApp, Width, Height);

		g_WebApp->Initialize();
		emscripten_set_main_loop(RunLopp, 60, true);
	}

	int main()
	{
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

	EMSCRIPTEN_KEEPALIVE
	void OnResize(int w, int h)
	{
		if (g_WebApp)
		{
			g_WebApp->OnResize(w, h);
		}
	}
}
#endif // __EMSCRIPTEN__