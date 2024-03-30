#pragma once
#include <iostream>
#include <string>

class Console
{
public:
	// フォーマット指定子と可変引数無し
	static void Log(const char* message) {
#if !defined(__EMSCRIPTEN__) && defined(_CONSOLE)
		_RPT0(_CRT_WARN, message);
#else
		// この関数を使う時に末尾に\nの改行を入れないとログが示されないので注意
		printf("%s", message);
#endif // !__EMSCRIPTEN__
	}

	static void Log(const wchar_t* message) {
#if !defined(__EMSCRIPTEN__) && defined(_CONSOLE)
		_RPTW0(_CRT_WARN, message);
#else
		// この関数を使う時に末尾に\nの改行を入れないとログが示されないので注意
		wprintf(L"%ls", message);
#endif // !__EMSCRIPTEN__
	}
	
	// フォーマット指定子と可変引数有り
	template<class... Variable>
	static void Log(const char* message, Variable&&... args) {
#if !defined(__EMSCRIPTEN__) && defined(_CONSOLE)
		_RPTN(_CRT_WARN, message, (args)...);
#else
		// この関数を使う時に末尾に\nの改行を入れないとログが示されないので注意
		printf(message, (args)...);
#endif // !__EMSCRIPTEN__
	}

	template<class... Variable>
	static void Log(const wchar_t* message, Variable&&... args) {
#if !defined(__EMSCRIPTEN__) && defined(_CONSOLE)
		_RPTWN(_CRT_WARN, message, (args)...);
#else
		// この関数を使う時に末尾に\nの改行を入れないとログが示されないので注意
		wprintf(message, (args)...);
#endif // !__EMSCRIPTEN__
	}
};