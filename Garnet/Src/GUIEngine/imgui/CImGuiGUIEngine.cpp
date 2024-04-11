#ifdef USE_GUIENGINE

#include "CImGuiGUIEngine.h"

#include <imgui.h>
#ifdef USE_GLFW
#include <imgui_impl_glfw.h>
#elif USE_WIN32_WindowAPI
#include <imgui_impl_win32.h>
#endif // USE_GLFW

#ifdef USE_VULKAN
#include <imgui_impl_vulkan.h>
#elif USE_WEBGPU
#include <imgui_impl_wgpu.h>
#elif USE_OPENGL
#include <imgui_impl_opengl3.h>
#include <imgui_impl_opengl3_loader.h>
#endif 

namespace gui
{
	CImGuiGUIEngine::CImGuiGUIEngine()
	{
	}

	CImGuiGUIEngine::~CImGuiGUIEngine()
	{
	}

#ifdef USE_GLFW
#ifdef USE_VULKAN
	bool CImGuiGUIEngine::Initialize_GLFW_Vulkan(GLFWwindow* pWindow)
	{
		// Dear ImGuiのコンテキストを作成
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard

		// Window APIやGraphics Backendの設定
		ImGui_ImplGlfw_InitForVulkan(pWindow, true);
		//ImGui_ImplVulkan_Init

		return true;
	}
#endif
#ifdef USE_WEBGPU
	bool CImGuiGUIEngine::Initialize_GLFW_WebGPU(GLFWwindow* pWindow)
	{
		return true;
	}
#endif
#endif // USE_GLFW

#ifdef USE_WIN32_WindowAPI
	bool CImGuiGUIEngine::Initialize_Win32API_OpenGL(HWND window)
	{
		return true;
	}
#endif // USE_WIN32_WindowAPI
}
#endif // USE_GUIENGINE