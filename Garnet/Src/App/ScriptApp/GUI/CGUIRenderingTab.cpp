#ifdef USE_GUIENGINE
#include "CGUIRenderingTab.h"

namespace gui
{
	CGUIRenderingTab::CGUIRenderingTab()
	{
	}

	bool CGUIRenderingTab::Draw()
	{
		if (ImGui::BeginTabItem("Rendering"))
		{
			ImGui::Text("This is Rendering Window");
			ImGui::EndTabItem();
		}

		return true;
	}
}
#endif