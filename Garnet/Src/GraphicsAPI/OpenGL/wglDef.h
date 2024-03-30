// シングルファイルで宣言と実装でそれぞれ一回ずつ読みたいのでここはコメントアウト. 3回目以降はプリプロセッサが止めてくれる
//#pragma once

// WGLはこっち
// Reference: https://registry.khronos.org/OpenGL/extensions/ARB/WGL_ARB_create_context.txt

/*#ifdef USE_OPENGL

#define NOMINMAX
#define WIN32_LEAN_AND_MEAD
#include <Windows.h>

#include "../../Message/Console.h"

#ifndef WGL_H //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define WGL_H

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126
#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

#include <gl/GL.h>

#define WGL_FUNC_LIST \
	WGL_FUNC(HGLRC,	CreateContextAttribsARB, HDC hDC, HGLRC hShareContext, const int* attribList) \
/* end */

#define WGL_FUNC(ret, name, ...) typedef ret WINAPI name##proc(__VA_ARGS__); extern name##proc * wgl##name;
WGL_FUNC_LIST
#undef WGL_FUNC

bool InitWGL();

#endif // !WGL_H //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WGL_IMPLEMENTATION

#define WGL_FUNC(ret, name, ...) name##proc * wgl##name;
WGL_FUNC_LIST
#undef WGL_FUNC

// OpenGLコンテキストの初期化関数
bool InitWGL()
{
	// OpenGLのdllの読み込み
	HINSTANCE dll = LoadLibraryA("opengl32.dll");
	typedef PROC WINAPI wglGetProcAddressproc(LPCSTR lpszProc);

	if (!dll)
	{
		Console::Log("[ERROR] InitWGL Error - dll not found\n");
		return false;
	}

	wglGetProcAddressproc* wglGetProcAddress = (wglGetProcAddressproc*)GetProcAddress(dll, "wglGetProcAddress");

#define WGL_FUNC(ret, name, ...) \
		wgl##name = (name##proc *)wglGetProcAddress("wgl" #name); \
		if(!wgl##name) { \
			Console::Log("[ERROR] wgl" #name "could=t loaded from opengl32.dll\n"); \
			return false; \
		} \
		else \
		{ \
			Console::Log("[Success] wgl" #name " Loaded\n"); \
		} 
		WGL_FUNC_LIST
#undef WGL_FUNC

	return true;
}

#endif // WGL_IMPLEMENTATION

#endif // USE_OPENGL*/