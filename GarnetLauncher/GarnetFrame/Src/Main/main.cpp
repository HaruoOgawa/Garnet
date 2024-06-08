#include <AppCore/CAppRunner.h>
#include "../App/CApp.h"

int main()
{
	app::SAppSettings Settings = {};
	Settings.FullScreen = false;

	if (!app::CAppRunner::Run(std::make_shared<app::CApp>(), Settings)) return 1;

	return 0;
}