#ifdef USE_GLFW

#include "../../AppCore/CAppCore.h"
#include "../../Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <Windows.h>
#endif

extern "C" {

app::CAppCore* g_AppCore = nullptr;

void Release()
{
	delete g_AppCore;
	g_AppCore = nullptr;
}

#ifdef __EMSCRIPTEN__
void RunLoop()
#else
bool RunLoop()
#endif // __EMSCRIPTEN__
{
	if (!g_AppCore->RunLoop() || !g_AppCore->IsRunLoop())
	{
		Release();

#ifndef __EMSCRIPTEN__
		return false;
#endif // !__EMSCRIPTEN__
	}

#ifndef __EMSCRIPTEN__
	return true;
#endif // !__EMSCRIPTEN__
}

#ifdef __EMSCRIPTEN__
// InputŒn‚ÍGLFW‚É”C‚¹‚é‚Ì‚Å‰½‚à‚µ‚È‚¢
EMSCRIPTEN_KEEPALIVE
void OnKeyDown(char* key)
{
}

EMSCRIPTEN_KEEPALIVE
void OnKeyUp(char* key)
{
}

EMSCRIPTEN_KEEPALIVE
void OnResize(int w, int h)
{
}

EMSCRIPTEN_KEEPALIVE
void OnMouseDown(int buttonNum, int x, int y)
{
}

EMSCRIPTEN_KEEPALIVE
void OnMouseUp(int buttonNum, int x, int y)
{
}

EMSCRIPTEN_KEEPALIVE
void OnMouseMove(int x, int y)
{
}

EMSCRIPTEN_KEEPALIVE
void OnMouseWheel(int deltaY)
{
}

EMSCRIPTEN_KEEPALIVE
#endif
void StartApp()
{
	g_AppCore = new app::CAppCore();
	
	if (g_AppCore->Initialize(1920, 1080))
	{
#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop(RunLoop, 60, true);
#else
		while (g_AppCore->IsRunLoop())
		{
			if (!RunLoop()) break;
		}
#endif // __EMSCRIPTEN__
	}
	else
	{
		Release();
	}
}

#if defined(__EMSCRIPTEN__) || defined(__CMAKE__)
int main()
#else
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
#ifndef __EMSCRIPTEN__
	StartApp();
#endif // !__EMSCRIPTEN__

	return 0;
}

}

#endif // USE_GLFW