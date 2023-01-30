#include "../../App/CApp.h"

#ifndef __EMSCRIPTEN__
app::CApp* g_App = nullptr;
#endif // !__EMSCRIPTEN__

int WinMain()
{
#ifndef __EMSCRIPTEN__
	g_App = new app::CApp();

	g_App->Initialize();
	g_App->RunLopp();

	delete g_App;
#endif // !__EMSCRIPTEN__
	
	return 0;
}