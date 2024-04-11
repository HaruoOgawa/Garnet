#pragma once

#ifdef USE_GLFW
#include <glfw3.h>
#include <glfw3native.h>
#endif

#ifdef USE_WIN32_WindowAPI
#include <Windows.h>
#endif

namespace gui
{
	class IGUIEngine
	{
	public:
#ifdef USE_GLFW
#ifdef USE_VULKAN
		virtual bool Initialize_GLFW_Vulkan(GLFWwindow* pWindow) = 0;
#endif
#ifdef USE_WEBGPU
		virtual bool Initialize_GLFW_WebGPU(GLFWwindow* pWindow) = 0;
#endif
#endif // USE_GLFW

#ifdef USE_WIN32_WindowAPI
		virtual bool Initialize_Win32API_OpenGL(HWND window) = 0;
#endif // USE_WIN32_WindowAPI
		
	};
}