#include "CAppRunner.h"
#include "CAppCore.h"
#include "../../Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__

extern "C"
{
	app::CAppCore* g_AppCore = nullptr;
	bool g_RunLoopResult = true;

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

			g_RunLoopResult = false;
		}
		else
		{
			g_RunLoopResult = true;
		}
	}

#ifdef __EMSCRIPTEN__
	EMSCRIPTEN_KEEPALIVE
#endif // __EMSCRIPTEN__
		void StartApp()
	{
		if (g_AppCore->Initialize(1920, 1080))
		{
#ifdef __EMSCRIPTEN__
			emscripten_set_main_loop(RunLoop, 60, true);
#else
			while (g_AppCore->IsRunLoop())
			{
				RunLoop();

				if (!g_RunLoopResult) break;
			}
#endif // __EMSCRIPTEN__
		}
		else
		{
			Release();
		}
	}

#ifdef __EMSCRIPTEN__
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
		void OnFocus(int focused)
	{
		if (g_AppCore)
		{
			g_AppCore->OnFocus(focused);
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
#endif // __EMSCRIPTEN__

	namespace app
	{
		bool CAppRunner::Run(const std::shared_ptr<app::IApp>& App)
		{
			g_AppCore = new app::CAppCore(App);

			// EmscriptenÇ≈ÇÕå„ÇŸÇ«JSÇ©ÇÁStartAppÇé¿çsÇ∑ÇÈ
#ifndef __EMSCRIPTEN__
			StartApp();
#endif // !__EMSCRIPTEN__

			return true;
		}
	}
}