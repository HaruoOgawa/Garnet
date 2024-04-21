#ifdef USE_GUIENGINE
#include "CGUITransformTab.h"
#include "../../Object/C3DObject.h"

namespace gui
{
	bool CGUITransformTab::Draw(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, int SelectedObjectIndex, int SelectedNodeIndex)
	{
		if (ImGui::BeginTabItem("Transform"))
		{
			if (SelectedObjectIndex >= 0 && SelectedObjectIndex < static_cast<int>(ObjectList.size()))
			{
				const auto& Object = ObjectList[SelectedObjectIndex];

				const auto& NodeList = Object->GetNodeList();

				if (SelectedNodeIndex >= 0 && SelectedNodeIndex < static_cast<int>(NodeList.size()))
				{
					ImGui::SeparatorText("Node Transform");

					// Node Transform‚ð•\Ž¦
					const auto& Node = NodeList[SelectedNodeIndex];
					const auto& Transform = Node->GetLocalTransform();

					if (!DrawTransformGUI(Transform)) return false;
				}
				else
				{
					ImGui::SeparatorText("Object Transform");

					// Object Transform‚ð•\Ž¦
					const auto& Transform = Object->GetObjectTransform();

					if (!DrawTransformGUI(Transform)) return false;
				}
			}

			ImGui::EndTabItem();
		}

		return true;
	}

	bool CGUITransformTab::DrawTransformGUI(const std::shared_ptr<math::CTransform>& Transform)
	{
		// Pos
		glm::vec3 Pos = Transform->GetPos();

		if(ImGui::InputFloat3("Position", &Pos[0]))
		{
			Transform->SetPos(Pos);
		}

		// Rotate
		glm::quat Rot = Transform->GetRot();
		glm::vec3 Euler = glm::eulerAngles(Rot);

		if (ImGui::InputFloat3("Rotation", &Euler[0]))
		{
			Rot = glm::angleAxis(Euler.z, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::angleAxis(Euler.y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(Euler.x, glm::vec3(1.0f, 0.0f, 0.0f));

			Transform->SetRot(Rot);
		}

		// Scale
		glm::vec3 Scale = Transform->GetScale();

		if (ImGui::InputFloat3("Scale", &Scale[0]))
		{
			Transform->SetScale(Scale);
		}

		return true;
	}
}
#endif