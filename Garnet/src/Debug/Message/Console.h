#pragma once
#include <iostream>
#include <string>

class Console
{
public:
	template<class... Variable>
	static void Log(const char* message, Variable&&... args) {
#ifndef __EMSCRIPTEN__
		_RPTN(_CRT_WARN, message, (args)...);
#else
		printf("%s", message, (args)...);
#endif // !__EMSCRIPTEN__
	}
};