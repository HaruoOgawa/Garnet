#ifdef USE_WEB_NATIVE
#include "../../AppCore/CAppCore.h"
#include "../../Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__

extern "C"
{
	app::CAppCore* g_AppCore = nullptr;

	void Release()
	{
		delete g_AppCore;
		g_AppCore = nullptr;
	}

	void RunLoop()
	{
		if (!g_AppCore->RunLoop() || !g_AppCore->IsRunLoop())
		{
			Release();
		}
	}

	EMSCRIPTEN_KEEPALIVE
	void StartApp(int Width, int Height)
	{
		g_AppCore = new app::CAppCore();

		g_AppCore->Initialize(Width, Height);
		emscripten_set_main_loop(RunLoop, 60, true);
	}

	int main()
	{
		return 0;
	}

	EMSCRIPTEN_KEEPALIVE
	void OnKeyDown(char* key)
	{
		if (g_AppCore)
		{
			g_AppCore->OnKeyDown(std::string(key));
		}
	}
	
	EMSCRIPTEN_KEEPALIVE
	void OnKeyUp(char* key)
	{
		if (g_AppCore)
		{
			g_AppCore->OnKeyUp(std::string(key));
		}
	}

	EMSCRIPTEN_KEEPALIVE
	void OnResize(int w, int h)
	{
		if (g_AppCore)
		{
			g_AppCore->OnResize(w, h);
		}
	}

	EMSCRIPTEN_KEEPALIVE
	void OnMouseDown(int buttonNum, int x, int y)
	{
		if (g_AppCore)
		{
			g_AppCore->OnMouseDown(buttonNum, x, y);
		}
	}
	
	EMSCRIPTEN_KEEPALIVE
	void OnMouseUp(int buttonNum, int x, int y)
	{
		if (g_AppCore)
		{
			g_AppCore->OnMouseUp(buttonNum, x, y);
		}
	}

	EMSCRIPTEN_KEEPALIVE
	void OnMouseMove(int x, int y)
	{
		if (g_AppCore)
		{
			g_AppCore->OnMouseMove(x, y);
		}
	}
	
	EMSCRIPTEN_KEEPALIVE
	void OnMouseWheel(int deltaY)
	{
		if (g_AppCore)
		{
			g_AppCore->OnMouseWheel(deltaY);
		}
	}
}
#endif // USE_WEB_NATIVE