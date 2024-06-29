#pragma once

#ifdef USE_GLFW
#include <glfw3.h>
#include <glfw3native.h>
#elif USE_WIN32_WindowAPI
#include <Windows.h>
#endif

#include <memory>

#include "IGraphicsAPI.h"
#include "IWindowAPI.h"
#include "../GUIEngine/imgui/Core/IImGuiCore.h"

namespace gui
{
	class IGUIEngine
	{
	public:
		virtual bool Initialize(window::IWindowAPI* pWindowAPI, api::IGraphicsAPI* pGraphicsAPI) = 0;
#ifdef USE_WIN32_WindowAPI
		virtual bool CheckInput(HWND window, UINT msg, WPARAM w_param, LPARAM l_param) = 0;
#endif
		virtual void Release(api::IGraphicsAPI* pGraphicsAPI) = 0;

		virtual bool BeginFrame(api::IGraphicsAPI* pGraphicsAPI) = 0;

		virtual bool EndFrame(api::IGraphicsAPI* pGraphicsAPI) = 0;

		virtual bool IsExistMouseOnGUI() = 0;

		virtual std::shared_ptr<IImGuiCore> GetImGuiCore() const = 0;
	};
}