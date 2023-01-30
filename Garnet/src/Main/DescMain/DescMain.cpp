#include "../../Debug/Message/Console.h"
#include "../../App/CApp.h"

int WinMain()
{
	Console::Log("Hello World\n");

	app::CApp app;
	app.Initialize();
	app.RunLopp();
	app.Release();

	return 0;
}