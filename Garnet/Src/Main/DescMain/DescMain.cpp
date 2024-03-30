#if (defined(USE_VULKAN) || defined(USE_WEBGPU)) && !defined(__EMSCRIPTEN__)

#include "../../App/CDescAppManager.h"
#include "../../App/EAppType.h"
#include "../../Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5_webgpu.h>
#elif defined(USE_WEBGPU) && defined(__CMAKE__)
#else
#include <Windows.h>
#endif

extern "C" {

descapp::CDescAppManager* g_DescApp = nullptr;

bool RunLopp()
{
	if(!g_DescApp->RunLopp()) return false;

	return true;
}

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
#ifndef __EMSCRIPTEN__
		while (g_DescApp->IsRunLoop())
		{
			if (!RunLopp()) break;
		}
#else
		emscripten_set_main_loop(RunLopp, 60, true);
#endif
		
	}

	delete g_DescApp;
	g_DescApp = nullptr;
}

#if defined(USE_WEBGPU) && defined(__CMAKE__)
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

#endif // USE_VULKAN