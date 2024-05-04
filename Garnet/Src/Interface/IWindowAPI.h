#pragma once

#ifdef USE_GLFW
#include <glfw3.h>
#include <glfw3native.h>
#elif USE_WIN32_WindowAPI
#include <Windows.h>
#endif

class IWindowAPI
{
public:
#ifdef USE_GLFW
	virtual GLFWwindow* GetGLFWWindow() const = 0;
#elif USE_WIN32_WindowAPI
	virtual const HWND& GetWin32Window() const = 0;
#endif
};