#ifdef USE_GUIENGINE
#include "CGUIObjectTab.h"

namespace gui
{
	CGUIObjectTab::CGUIObjectTab()
	{
	}

	bool CGUIObjectTab::Draw()
	{
		if (ImGui::BeginTabItem("Object"))
		{
			ImGui::Text("This is Object Window");

			if (ImGui::BeginChild("ObjectListChild", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.25f), ImGuiChildFlags_Border, 0))
			{
				ImGui::Text("Object List Box");

				if (ImGui::TreeNodeEx((void*)(intptr_t)-1, ImGuiTreeNodeFlags_OpenOnArrow, "glTFObject"))
				{
					for (int i = 0; i < 50; i++)
					{
						int index = i * 3;

						if (ImGui::TreeNodeEx((void*)(intptr_t)index, ImGuiTreeNodeFlags_OpenOnArrow, "aaa"))
						{
							for (int m = 0; m < 3; m++)
							{
								if (ImGui::TreeNodeEx((void*)(intptr_t)(index + m), ImGuiTreeNodeFlags_OpenOnArrow, "bbb"))
								{
									ImGui::TreePop();
								}

								ImGui::SameLine();
								static bool Flag = true;
								ImGui::Checkbox("", &Flag);

								ImGui::SameLine();
								std::string Name = "ChildNode " + std::to_string(i);
								ImGui::Button("Show");
								//ImGui::Button(Name.c_str(), ImVec2(-FLT_MIN, 0.0f));
							}

							ImGui::TreePop();
						}

						ImGui::SameLine();
						static bool Flag = true;
						ImGui::Checkbox("", &Flag);

						ImGui::SameLine();
						std::string Name = "Node " + std::to_string(i);
						ImGui::Button("Show");
						//ImGui::Button(Name.c_str(), ImVec2(-FLT_MIN, 0.0f));
					}

					ImGui::TreePop();
				}

				/*for (int i = 0; i < 100; i++)
					ImGui::Text("%04d: scrollable region", i);*/

				ImGui::EndChild();
			}

			// ImGui::GetContentRegionAvail().y * 0.5‚Æ‚©‚ÌŒã‚É0‚É‚µ‚½‚ç‚È‚ñ‚©‚µ‚ç‚ñ‚ª‚¢‚¢Š´‚¶‚É‚Ò‚Á‚½‚è‚ÌˆÊ’u‚É”z’u‚µ‚Ä‚­‚ê‚é
			if (ImGui::BeginChild("ObjectDetailChild", ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Border, 0))
			{
				if (ImGui::BeginTabBar("ObjectDetail"))
				{
					if (ImGui::BeginTabItem("Material"))
					{
						ImGui::Text("This is Material Window");
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Transform"))
					{
						ImGui::Text("This is Transform Window");
						ImGui::EndTabItem();
					}

					ImGui::EndTabBar(); // ObjectDetail
				}

				ImGui::EndChild();
			}

			ImGui::EndTabItem();
		}

		return true;
	}
}
#endif