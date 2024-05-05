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

		virtual bool Initialize(window::IWindowAPI* pWindowAPI, api::IGraphicsAPI* pGraphicsAPI) override;
#ifdef USE_WIN32_WindowAPI
		virtual bool CheckInput(HWND window, UINT msg, WPARAM w_param, LPARAM l_param) override;
#endif
		virtual void Release(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool BeginFrame(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool EndFrame(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool IsExistMouseOnGUI() override;
	};
}