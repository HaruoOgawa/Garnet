#pragma once

#ifdef USE_GUIENGINE

#include <string>
#include <imgui.h>

#include "CGUIObjectTab.h"
#include "CGUIRenderingTab.h"
#include "CGUICameraTab.h"
#include "CGUIResourcesTab.h"

#include "../../App/ScriptApp/CScriptApp.h"

namespace api { class IGraphicsAPI; }

namespace gui
{
	class CGraphicsEditingWindow
	{
		CGUIObjectTab m_GUIObjectTab;
		CGUIResourcesTab m_GUIResourcesTab;
	public:
		CGraphicsEditingWindow();
		virtual ~CGraphicsEditingWindow();

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, const app::CScriptApp* pApp);
	};
}
#endif // USE_GUIENGINE