#include "CDummyGUIEngine.h"

namespace gui
{
	CDummyGUIEngine::CDummyGUIEngine()
	{
	}

	CDummyGUIEngine::~CDummyGUIEngine()
	{
	}

	void CDummyGUIEngine::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
	}

#ifdef USE_GLFW
	bool CDummyGUIEngine::InitializeWithGLFW(GLFWwindow* pWindow, api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}
#elif USE_WIN32_WindowAPI
	bool CDummyGUIEngine::InitializeWithWin32API(HWND window, api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CDummyGUIEngine::CheckInput(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
	{
		return false;
	}
#endif

	bool CDummyGUIEngine::BeginFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CDummyGUIEngine::EndFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CDummyGUIEngine::IsExistMouseOnGUI()
	{
		return false;
	}
}