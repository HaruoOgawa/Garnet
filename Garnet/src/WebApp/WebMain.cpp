#include <iostream>
#include "../Test/TestOther.h"
#include "../Test/TestSubFolder/TestSubClass.h"

int main()
{
	std::cout << "Hello Garnet WASM" << std::endl;

	TestOther::CallTestMessage();
	TestSubClass::SubCall();

	return 0;
}