#ifdef USE_GUIENGINE
#include "CGUIMaterialTab.h"
#include "../../Object/C3DObject.h"

namespace gui
{
	bool CGUIMaterialTab::Draw(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, int SelectedObjectIndex, int SelectedNodeIndex)
	{
		if (ImGui::BeginTabItem("Material"))
		{
			ImGui::Text("This is Material Window");

			std::string TestText = "SelectedObjectIndex: " + std::to_string(SelectedObjectIndex) + " / " + "SelectedNodeIndex: " + std::to_string(SelectedNodeIndex);
			ImGui::Text(TestText.c_str());

			ImGui::EndTabItem();
		}

		return true;
	}
}
#endif