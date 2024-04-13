#pragma once

#ifdef USE_GUIENGINE

#include <memory>
#include "../../Interface/IGUIEngine.h"
#include "Core/IImGuiCore.h"

namespace gui
{
	class CImGuiGUIEngine : public IGUIEngine
	{
		std::shared_ptr<IImGuiCore> m_ImGuiCore;
	public:
		CImGuiGUIEngine();
		virtual ~CImGuiGUIEngine();

#ifdef USE_GLFW
		virtual bool InitializeWithGLFW(GLFWwindow* pWindow, api::IGraphicsAPI* pGraphicsAPI) override;
#elif USE_WIN32_WindowAPI
		virtual bool InitializeWithWin32API(HWND window, api::IGraphicsAPI* pGraphicsAPI) override;
#endif
		virtual void Release(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool BeginFrame(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool EndFrame(api::IGraphicsAPI* pGraphicsAPI) override;
	};
}

#endif // USE_GUIENGINE