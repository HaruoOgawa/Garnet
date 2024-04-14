#pragma once

#include <memory>
#include "../../Interface/IGUIEngine.h"
#include "../../Interface/IGraphicsAPI.h"

namespace gui
{
	class CDummyGUIEngine : public IGUIEngine
	{
	public:
		CDummyGUIEngine();
		virtual ~CDummyGUIEngine();

#ifdef USE_GLFW
		virtual bool InitializeWithGLFW(GLFWwindow* pWindow, api::IGraphicsAPI* pGraphicsAPI) override;
#elif USE_WIN32_WindowAPI
		virtual bool InitializeWithWin32API(HWND window, api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool CheckInput(HWND window, UINT msg, WPARAM w_param, LPARAM l_param) override;
#endif
		virtual void Release(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool BeginFrame(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool EndFrame(api::IGraphicsAPI* pGraphicsAPI) override;
	};
}