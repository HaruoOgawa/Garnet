#pragma once

// Reference: https://github.com/ApoorvaJ/Papaya/blob/3808e39b0f45d4ca4972621c847586e4060c042a/src/libs/gl_lite.h#L113

#ifdef USE_OPENGL

#ifndef GL_H
#define GL_H

#define NOMINMAX
#define WIN32_LEAN_AND_MEAD
#include <Windows.h>

#include "../../Debug/Message/Console.h"

// OpenGLの定数を定義
#define GL_ARRAY_BUFFER                   0x8892 // Acquired from:
#define GL_ARRAY_BUFFER_BINDING           0x8894 // https://www.opengl.org/registry/api/GL/glext.h
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COMPILE_STATUS                 0x8B81
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_FRAMEBUFFER                    0x8D40
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FUNC_ADD                       0x8006
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_STATIC_DRAW                    0x88E4
#define GL_STREAM_DRAW                    0x88E0
#define GL_TEXTURE0                       0x84C0
#define GL_VERTEX_SHADER                  0x8B31

typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#include <gl/GL.h>

// OpenGLの関数を定義
#define GL_FUNC_LIST \
	GL_FUNC(void, AttachShader, GLuint program, GLuint shader) \
	GL_FUNC(void, BindBuffer,	GLenum target, GLuint buffer) \
/* end */

// プリプロセッサ芸でまずGL_FUNC_LISTの中身をtypedef や externで展開する
#define GL_FUNC(ret, name, ...) typedef ret WINAPI name##proc(__VA_ARGS__); extern name##proc * gl##name;
GL_FUNC_LIST
#undef GL_FUNC

bool InitGL();
#endif // !GL_H

#ifdef GL_IMPLEMENTATION
// OpenGLコンテキストの初期化関数
bool InitGL()
{
	// OpenGLのdllの読み込み
	HINSTANCE dll = LoadLibraryA("opengl32.dll");
	typedef PROC WINAPI wglGetProcAddressproc(LPCSTR lpszProc);

	if (!dll)
	{
		Console::Log("[ERROR] InitGL Error - dll not found\n");
		return false;
	}

	wglGetProcAddressproc* wglGetProcAddress = (wglGetProcAddressproc*)GetProcAddress(dll, "wglGetProcAddress");

	// dllに定義されている関数と自身がtypedefした関数を結び付ける
	// この中でプリプロセッサを一気に展開している(プリプロセッサ芸ってやつ？・・・)
	// 改行文字を入れつつ複数行のプログラミングをしているのか・・・
	// 次のGL_FUNC_LISTをProcと結びつけるために展開する
#define GL_FUNC(ret, name, ...) \
		gl##name = (name##proc *)wglGetProcAddress("gl" #name); \
		if(!gl##name) { \
			Console::Log("[ERROR] gl" #name "could=t loaded from opengl32.dll\n"); \
			return false; \
		} \
		GL_FUNC_LIST
#undef GL_FUNC

	return true;
}
#endif // GL_IMPLEMENTATION


#endif // USE_OPENGL