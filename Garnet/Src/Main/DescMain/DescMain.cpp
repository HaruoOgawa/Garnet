#ifdef USE_GLFW

#include "../../WindowAPI/CGLFWWindowAPI.h"
#include "../../Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <Windows.h>
#endif

extern "C" {

descapp::CGLFWWindowAPI* g_DescApp = nullptr;

void Release()
{
	delete g_DescApp;
	g_DescApp = nullptr;
}

#ifdef __EMSCRIPTEN__
void RunLopp()
#else
bool RunLopp()
#endif // __EMSCRIPTEN__
{
	if (!g_DescApp->RunLopp() || !g_DescApp->IsRunLoop())
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
	g_DescApp = new descapp::CGLFWWindowAPI();
	
	if (g_DescApp->Initialize())
	{
#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop(RunLopp, 60, true);
#else
		while (g_DescApp->IsRunLoop())
		{
			if (!RunLopp()) break;
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