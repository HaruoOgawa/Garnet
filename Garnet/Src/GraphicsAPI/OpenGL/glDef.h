// シングルファイルで宣言と実装でそれぞれ一回ずつ読みたいのでここはコメントアウト. 3回目以降はプリプロセッサが止めてくれる
//#pragma once

// Reference: https://github.com/ApoorvaJ/Papaya/blob/3808e39b0f45d4ca4972621c847586e4060c042a/src/libs/gl_lite.h#L113

#ifdef USE_OPENGL

#define NOMINMAX
#define WIN32_LEAN_AND_MEAD
#include <Windows.h>

#include "../../Message/Console.h"

#ifndef GL_H //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define GL_H

// OpenGLの定数を定義
#define GL_ARRAY_BUFFER                   0x8892 // Acquired from:
#define GL_ARRAY_BUFFER_BINDING           0x8894 // https://www.opengl.org/registry/api/GL/glext.h
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COMPILE_STATUS                 0x8B81
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895

#define GL_FRAMEBUFFER                    0x8D40
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FUNC_ADD                       0x8006
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_TESS_CONTROL_SHADER            0x8E88
#define GL_TESS_EVALUATION_SHADER         0x8E87
#define GL_COMPUTE_SHADER                 0x91B9
#define GL_UNIFORM_BUFFER                 0x8A11
#define GL_STATIC_DRAW                    0x88E4
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_RGBA16F                        0x881A
#define GL_DEPTH24_STENCIL8               0x88F0
#define GL_DEPTH_STENCIL                  0x84F9
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP       0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define GL_TEXTURE_CUBE_MAP_SEAMLESS      0x884F
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_LINK_STATUS                    0x8B82
#define GL_DEPTH_COMPONENT32F             0x8CAC
#define GL_RENDERBUFFER                   0x8D41
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_SHADER_STORAGE_BUFFER          0x90D2
#define GL_SHADER_STORAGE_BLOCK           0x92E6

typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#include <gl/GL.h>

// OpenGLの関数を定義
#define GL_FUNC_LIST \
	GL_FUNC(void,	BindBuffer,			  GLenum target, GLuint buffer) \
	GL_FUNC(GLuint, CreateProgram) \
	GL_FUNC(void,   GetProgramiv,		  GLuint program, GLenum pname, GLint* params) \
	GL_FUNC(void,	UseProgram,			  GLuint program) \
	GL_FUNC(void,	DeleteProgram,		  GLuint program) \
	GL_FUNC(void,	GetProgramInfoLog,	  GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog) \
	GL_FUNC(GLuint, CreateShader,		  GLenum shaderType) \
	GL_FUNC(void,	ShaderSource,		  GLuint shader, GLsizei count, const GLchar** string, const GLint* length) \
	GL_FUNC(void ,  CompileShader,		  GLuint shader) \
	GL_FUNC(void,	AttachShader,		  GLuint program, GLuint shader) \
	GL_FUNC(void,	DeleteShader,		  GLuint shader) \
	GL_FUNC(void,	GetShaderiv,		  GLuint shader, GLenum pname, GLint* params) \
	GL_FUNC(void,	GetShaderInfoLog,	  GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog) \
	GL_FUNC(void,   GenBuffers, 		  GLsizei n, GLuint* buffers) \
	GL_FUNC(void,   BufferData, 		  GLenum target, GLsizeiptr size, const void* data, GLenum usage) \
	GL_FUNC(void,   BufferSubData, 		  GLenum target, GLintptr offset, GLsizeiptr size, const void* data) \
	GL_FUNC(GLuint, GetUniformBlockIndex, GLuint program, const GLchar* uniformBlockName) \
	GL_FUNC(GLuint, GetProgramResourceIndex, GLuint program, GLenum programInterface, const char* name) \
	GL_FUNC(void,   UniformBlockBinding,  GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding) \
	GL_FUNC(void,   ShaderStorageBlockBinding,  GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding) \
	GL_FUNC(void,   BindBufferBase, 	  GLenum target, GLuint index, GLuint buffer) \
	GL_FUNC(void,   BindBufferRange, 	  GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) \
	GL_FUNC(void,   GenVertexArrays, 	  GLsizei n, GLuint* arrays) \
	GL_FUNC(void,   BindVertexArray, 	  GLuint array) \
	GL_FUNC(void,   DeleteVertexArrays,   GLsizei n, const GLuint* arrays) \
	GL_FUNC(void,   EnableVertexAttribArray, GLuint index) \
	GL_FUNC(void,   VertexAttribPointer,  GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) \
	GL_FUNC(void,   VertexAttribIPointer, GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer) \
	GL_FUNC(void,   LinkProgram,		  GLuint program) \
	GL_FUNC(void,   ActiveTexture,		  GLenum texture) \
	GL_FUNC(void,   GenerateMipmap,		  GLenum target) \
	GL_FUNC(GLint,  GetUniformLocation,	  GLuint program, const GLchar* name) \
	GL_FUNC(void,   Uniform1i,			  GLint location, GLint v0) \
	GL_FUNC(void,   GenFramebuffers,	  GLsizei n, GLuint* ids) \
	GL_FUNC(void,   BindFramebuffer,	  GLenum target, GLuint framebuffer) \
	GL_FUNC(void,   FramebufferTexture2D, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) \
	GL_FUNC(void,   GenRenderbuffers,	  GLsizei n, GLuint* renderbuffers) \
	GL_FUNC(void,   BindRenderbuffer,	  GLenum target, GLuint renderbuffer) \
	GL_FUNC(void,   RenderbufferStorage,  GLenum target, GLenum internalformat, GLsizei width, GLsizei height) \
	GL_FUNC(void,   FramebufferRenderbuffer, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) \
	GL_FUNC(void,	DrawElementsInstanced, 	GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount) \
	GL_FUNC(void,	DispatchCompute, 	  GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) \
/* end */

// プリプロセッサ芸でまずGL_FUNC_LISTの中身をtypedef や externで展開する
#define GL_FUNC(ret, name, ...) typedef ret WINAPI name##proc(__VA_ARGS__); extern name##proc * gl##name;
GL_FUNC_LIST
#undef GL_FUNC

bool InitGL();

#endif // !GL_H //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef GL_IMPLEMENTATION

#define GL_FUNC(ret, name, ...) name##proc * gl##name;
GL_FUNC_LIST
#undef GL_FUNC

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
	
	// あと最後の行には円マークは付けないように注意. 正常に処理が呼ばれなくなる
#define GL_FUNC(ret, name, ...) \
		gl##name = (name##proc *)wglGetProcAddress("gl" #name); \
		if(!gl##name) { \
			Console::Log("[ERROR] gl" #name "could=t loaded from opengl32.dll\n"); \
			return false; \
		} \
		else \
		{ \
			Console::Log("[Success] gl" #name " Loaded\n"); \
		} 
		GL_FUNC_LIST
#undef GL_FUNC

	return true;
}

#endif // GL_IMPLEMENTATION

#endif // USE_OPENGL