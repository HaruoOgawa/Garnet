#include "../Message/Console.h"
#include "../Test/TestOther.h"

int WinMain()
{
	Console::Log("Hello World\n");

	TestOther::CallTestMessage();

	return 0;
}