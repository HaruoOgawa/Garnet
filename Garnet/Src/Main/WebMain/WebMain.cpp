#ifdef USE_WEB_NATIVE
#include "../../AppManager/CWebAppManager.h"
#include "../../AppManager/EAppType.h"
#include "../../Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__

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
	void OnKeyUp(char* key)
	{
		if (g_WebApp)
		{
			g_WebApp->OnKeyUp(std::string(key));
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

	EMSCRIPTEN_KEEPALIVE
	void OnMouseDown(int buttonNum, int x, int y)
	{
		if (g_WebApp)
		{
			g_WebApp->OnMouseDown(buttonNum, x, y);
		}
	}
	
	EMSCRIPTEN_KEEPALIVE
	void OnMouseUp(int buttonNum, int x, int y)
	{
		if (g_WebApp)
		{
			g_WebApp->OnMouseUp(buttonNum, x, y);
		}
	}

	EMSCRIPTEN_KEEPALIVE
	void OnMouseMove(int x, int y)
	{
		if (g_WebApp)
		{
			g_WebApp->OnMouseMove(x, y);
		}
	}
	
	EMSCRIPTEN_KEEPALIVE
	void OnMouseWheel(int deltaY)
	{
		if (g_WebApp)
		{
			g_WebApp->OnMouseWheel(deltaY);
		}
	}
}
#endif // USE_WEB_NATIVE