#ifdef USE_GUIENGINE
#include "CGUITransformTab.h"
#include "../../Object/C3DObject.h"

namespace gui
{
	bool CGUITransformTab::Draw(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, int SelectedObjectIndex, int SelectedNodeIndex)
	{
		if (ImGui::BeginTabItem("Transform"))
		{
			ImGui::Text("This is Transform Window");

			std::string TestText = "SelectedObjectIndex: " + std::to_string(SelectedObjectIndex) + " / " + "SelectedNodeIndex: " + std::to_string(SelectedNodeIndex);
			ImGui::Text(TestText.c_str());

			ImGui::EndTabItem();
		}

		return true;
	}
}
#endif