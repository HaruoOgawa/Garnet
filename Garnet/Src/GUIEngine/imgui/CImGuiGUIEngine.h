#pragma once

#ifdef USE_GUIENGINE

#include <memory>
#include "../../Interface/IGUIEngine.h"
#include "Core/IImGuiCore.h"

#include <imgui.h>
#ifdef USE_GLFW
#include <imgui_impl_glfw.h>
#elif USE_WIN32_WindowAPI
#include <imgui_impl_win32.h>
#endif // USE_GLFW

#ifdef USE_VULKAN
#include "Core/CImGuiCoreVulkan.h"
#elif USE_WEBGPU
#include "Core/CImGuiCoreWebGPU.h"
#elif USE_OPENGL
#include "Core/CImGuiCoreOpenGL.h"
#endif 

#ifdef USE_WIN32_WindowAPI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // USE_WIN32_WindowAPI

namespace gui
{
	class CImGuiGUIEngine : public IGUIEngine
	{
		bool m_Initialized;

		std::shared_ptr<IImGuiCore> m_ImGuiCore;
	public:
		CImGuiGUIEngine();
		virtual ~CImGuiGUIEngine();

		virtual bool Initialize(window::IWindowAPI* pWindowAPI, api::IGraphicsAPI* pGraphicsAPI) override;
#ifdef USE_WIN32_WindowAPI
		virtual bool CheckInput(HWND window, UINT msg, WPARAM w_param, LPARAM l_param) override;
#endif
		virtual void Release(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool BeginFrame(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool EndFrame(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool IsExistMouseOnGUI() override;

		virtual std::shared_ptr<IImGuiCore> GetImGuiCore() const override;
	};
}

#endif // USE_GUIENGINE