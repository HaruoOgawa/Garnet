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

	bool CDummyGUIEngine::Initialize(window::IWindowAPI* pWindowAPI, api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

#ifdef USE_WIN32_WindowAPI
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

	std::shared_ptr<IImGuiCore> CDummyGUIEngine::GetImGuiCore() const
	{
		return nullptr;
	}
}