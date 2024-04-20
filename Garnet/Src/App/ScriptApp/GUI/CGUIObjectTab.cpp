#ifdef USE_GUIENGINE
#include "CGUIObjectTab.h"
#include "CGUIMaterialTab.h"
#include "CGUITransformTab.h"

#include "../../Object/C3DObject.h"

namespace gui
{
	CGUIObjectTab::CGUIObjectTab():
		m_SelectedObjectIndex(-1),
		m_SelectedNodeIndex(-1)
	{
	}

	bool CGUIObjectTab::Draw(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList)
	{
		if (ImGui::BeginTabItem("ObjectTabItem"))
		{
			if (ImGui::BeginChild("ObjectListChild", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.25f), ImGuiChildFlags_Border, 0))
			{
				for (int CurrentObjectIndex = 0; CurrentObjectIndex < static_cast<int>(ObjectList.size()); CurrentObjectIndex++)
				{
					const auto& Object = ObjectList[CurrentObjectIndex];

					//
					static bool Flag = true;
					ImGui::Checkbox("Enable", &Flag);

					ImGui::SameLine();
					std::string BtnLabel = "Show_" + Object->GetObjectName();
					if (ImGui::Button(BtnLabel.c_str()))
					{
						m_SelectedObjectIndex = CurrentObjectIndex;
						m_SelectedNodeIndex = -1;
					}

					// ObjectのTreeNodeを配置
					ImGui::SameLine();
					if (ImGui::TreeNodeEx(Object->GetObjectName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow))
					{
						// NodeListをTreeNodeに配置
						const auto& NodeList = Object->GetNodeList();

						const auto& RootNodeIndexList = Object->GetRootNodeIndexList();
						if (!RootNodeIndexList.empty())
						{
							for (const auto& SceneRootNodeList : RootNodeIndexList)
							{
								for (const int RootNodeIndex : SceneRootNodeList)
								{
									if (RootNodeIndex < 0 || RootNodeIndex >= NodeList.size()) continue;

									const auto& RootNode = NodeList[RootNodeIndex];
									if (!DrawNodeGUI(m_SelectedObjectIndex, m_SelectedNodeIndex, CurrentObjectIndex, RootNodeIndex, RootNode, NodeList)) return false;
								}
							}
						}
						else
						{
							for (int NodeIndex = 0; NodeIndex < static_cast<int>(NodeList.size()); NodeIndex++)
							{
								const auto& Node = NodeList[NodeIndex];

								if (!DrawNodeGUI(m_SelectedObjectIndex, m_SelectedNodeIndex, CurrentObjectIndex, NodeIndex, Node, NodeList)) return false;
							}
						}

						ImGui::TreePop();
					}
				}

				ImGui::EndChild();
			}

			// ImGui::GetContentRegionAvail().y * 0.5とかの後に0にしたらなんかしらんがいい感じにぴったりの位置に配置してくれる
			if (ImGui::BeginChild("ObjectDetailChild", ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Border, 0))
			{
				if (ImGui::BeginTabBar("ObjectDetail"))
				{
					if (!CGUIMaterialTab::Draw(ObjectList, m_SelectedObjectIndex, m_SelectedNodeIndex)) return false;
					if (!CGUITransformTab::Draw(ObjectList, m_SelectedObjectIndex, m_SelectedNodeIndex)) return false;

					ImGui::EndTabBar(); // ObjectDetail
				}

				ImGui::EndChild();
			}

			ImGui::EndTabItem();
		}

		return true;
	}

	bool CGUIObjectTab::DrawNodeGUI(int& SelectedObjectIndex, int& SelectedNodeIndex, int CurrentObjectIndex, int CurrentNodeIndex,
		const std::shared_ptr<object::CNode>& Node, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		//
		static bool Flag = true;
		ImGui::Checkbox("Enable", &Flag);

		ImGui::SameLine();
		std::string Label = "Show_" + Node->GetName();
		if (ImGui::Button(Label.c_str()))
		{
			SelectedObjectIndex = CurrentObjectIndex;
			SelectedNodeIndex = CurrentNodeIndex;
		}

		// GUIの描画
		ImGui::SameLine();
		if (ImGui::TreeNodeEx(Node->GetName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow))
		{
			// 子要素の走破
			for (const int ChildIndex : Node->GetChildrenNodeIndexList())
			{
				if (ChildIndex < 0 || ChildIndex >= NodeList.size()) continue;

				const auto& ChildNode = NodeList[ChildIndex];
				if (!DrawNodeGUI(SelectedObjectIndex, SelectedNodeIndex, CurrentObjectIndex, ChildIndex, ChildNode, NodeList)) return false;
			}

			ImGui::TreePop();
		}

		return true;
	}
}
#endif