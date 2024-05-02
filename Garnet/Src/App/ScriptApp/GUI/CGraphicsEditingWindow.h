#pragma once

#ifdef USE_GUIENGINE

#include <string>
#include <imgui.h>

#include "CGUIObjectTab.h"
#include "CGUIRenderingTab.h"
#include "CGUICameraTab.h"
#include "CGUIResourcesTab.h"

#include "../../Interface/IApp.h"

namespace gui
{
	class CGraphicsEditingWindow
	{
		CGUIObjectTab m_GUIObjectTab;
		CGUIResourcesTab m_GUIResourcesTab;
	public:
		CGraphicsEditingWindow();
		virtual ~CGraphicsEditingWindow();

		bool Draw(const app::IApp* pApp);
	};
}
#endif // USE_GUIENGINE