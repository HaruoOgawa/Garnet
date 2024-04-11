#include "CDummyGUIEngine.h"

namespace gui
{
	CDummyGUIEngine::CDummyGUIEngine()
	{
	}

	CDummyGUIEngine::~CDummyGUIEngine()
	{
	}

#ifdef USE_GLFW
#ifdef USE_VULKAN
	bool CDummyGUIEngine::Initialize_GLFW_Vulkan(GLFWwindow* pWindow)
	{
		return true;
	}
#endif
#ifdef USE_WEBGPU
	bool CDummyGUIEngine::Initialize_GLFW_WebGPU(GLFWwindow* pWindow)
	{
		return true;
	}
#endif
#endif // USE_GLFW

#ifdef USE_WIN32_WindowAPI
	bool CDummyGUIEngine::Initialize_Win32API_OpenGL(HWND window)
	{
		return true;
	}
#endif // USE_WIN32_WindowAPI
}