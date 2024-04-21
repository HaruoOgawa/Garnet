#ifdef USE_GUIENGINE
#include "CGUICameraTab.h"

namespace gui
{
	CGUICameraTab::CGUICameraTab()
	{
	}

	bool CGUICameraTab::Draw()
	{
		if (ImGui::BeginTabItem("Camera"))
		{
			ImGui::Text("This is Camera Window");
			ImGui::EndTabItem();
		}

		return true;
	}
}
#endif