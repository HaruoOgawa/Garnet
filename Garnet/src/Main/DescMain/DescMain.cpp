#include "../../App/CDescAppManager.h"
#include "../../App/EAppType.h"
#include "../../Debug/Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5_webgpu.h>
#elif defined(__DAWN__) && defined(__CMAKE__)
#else
#include <Windows.h>
#endif

extern "C" {

descapp::CDescAppManager* g_DescApp = nullptr;

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
void OnKeyDown(char* key)
{
	Console::Log("[OnKeyDown] key: %c\n", key);
}

EMSCRIPTEN_KEEPALIVE
#endif
void StartApp()
{
	g_DescApp = new descapp::CDescAppManager(app::EAppType::ScriptApp);
	
	if (g_DescApp->Initialize())
	{
		g_DescApp->RunLopp();
	}

	delete g_DescApp;
	g_DescApp = nullptr;
}

#if defined(__DAWN__) && defined(__CMAKE__)
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