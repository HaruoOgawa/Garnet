#include <AppCore/CAppRunner.h>
#include "../App/ScriptApp/CDevApp.h"

int main()
{
	app::SAppSettings Settings = {};
	Settings.FullScreen = false;

	if (!app::CAppRunner::Run(std::make_shared<app::CDevApp>(), Settings)) return 1;

	return 0;
}