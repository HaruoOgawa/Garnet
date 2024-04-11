#pragma once

#ifdef USE_GUIENGINE

#include "../../Interface/IGUIEngine.h"

namespace gui
{
	class CImGuiGUIEngine : public IGUIEngine
	{
	public:
		CImGuiGUIEngine();
		virtual ~CImGuiGUIEngine();

#ifdef USE_GLFW
#ifdef USE_VULKAN
		virtual bool Initialize_GLFW_Vulkan(GLFWwindow* pWindow) override;
#endif
#ifdef USE_WEBGPU
		virtual bool Initialize_GLFW_WebGPU(GLFWwindow* pWindow) override;
#endif
#endif // USE_GLFW

#ifdef USE_WIN32_WindowAPI
		virtual bool Initialize_Win32API_OpenGL(HWND window) override;
#endif // USE_WIN32_WindowAPI
	};
}

#endif // USE_GUIENGINE