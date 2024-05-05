#include "../AppCore/CAppRunner.h"
#include "../App/ScriptApp/CScriptApp.h"

int main()
{
	if (!app::CAppRunner::Run(std::make_shared<app::CScriptApp>())) return 1;

	return 0;
}