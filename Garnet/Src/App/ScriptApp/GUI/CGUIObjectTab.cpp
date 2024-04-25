#ifdef USE_GUIENGINE
#include "CGUIObjectTab.h"
#include "CGUIMaterialTab.h"
#include "CGUITransformTab.h"

#include "../../Object/C3DObject.h"

namespace gui
{
	CGUIObjectTab::CGUIObjectTab():
		m_SelectedObjectIndex(-1),
		m_SelectedNodeIndex(-1),
		m_SelectedName(""),
		m_OperateButtonID(-1)
	{
	}

	void CGUIObjectTab::Reset()
	{
		m_OperateButtonID = -1;
	}

	bool CGUIObjectTab::Draw(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList)
	{
		Reset();

		if (ImGui::BeginTabItem("Object"))
		{
			if (ImGui::BeginChild("ObjectListChild", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.25f), ImGuiChildFlags_Border, 0))
			{
				for (int CurrentObjectIndex = 0; CurrentObjectIndex < static_cast<int>(ObjectList.size()); CurrentObjectIndex++)
				{
					const auto& Object = ObjectList[CurrentObjectIndex];

					// OperateButton
					{
						m_OperateButtonID++;

						std::string BoxLabel = "##" + std::to_string(m_OperateButtonID);
						bool Flag = Object->IsEnabled();
						if (ImGui::Checkbox(BoxLabel.c_str(), &Flag))
						{
							Object->SetEnabled(Flag);
						}

						ImGui::SameLine();
					}

					// ObjectのTreeNodeを配置
					const bool IsOpend = ImGui::TreeNodeEx(Object->GetObjectName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Framed);
					
					if (ImGui::IsItemClicked())
					{
						m_SelectedObjectIndex = CurrentObjectIndex;
						m_SelectedNodeIndex = -1;

						m_SelectedName = Object->GetObjectName();
					}
					
					if(IsOpend)
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
				if (!m_SelectedName.empty())
				{
					ImGui::Text("%s", m_SelectedName.c_str());
				}

				if (ImGui::BeginTabBar("ObjectDetail"))
				{
					if (!CGUITransformTab::Draw(ObjectList, m_SelectedObjectIndex, m_SelectedNodeIndex)) return false;
					if (!CGUIMaterialTab::Draw(ObjectList, m_SelectedObjectIndex, m_SelectedNodeIndex)) return false;

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
		// GUIの描画
		// OperateButton
		{
			m_OperateButtonID++;

			std::string BoxLabel = "##" + std::to_string(m_OperateButtonID);
			bool Flag = Node->IsEnabled();
			if (ImGui::Checkbox(BoxLabel.c_str(), &Flag))
			{
				Node->SetEnabled(Flag);

				SetDrawable(Flag, Node, NodeList);
			}

			ImGui::SameLine();
		}

		//
		const bool IsOpened = ImGui::TreeNodeEx(Node->GetName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Framed);

		if (ImGui::IsItemClicked())
		{
			SelectedObjectIndex = CurrentObjectIndex;
			SelectedNodeIndex = CurrentNodeIndex;

			m_SelectedName = Node->GetName();
		}

		if (IsOpened)
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

	void CGUIObjectTab::SetDrawable(bool Flag, const std::shared_ptr<object::CNode>& Node, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		Node->SetDrawable(Flag);

		// 子要素の走破
		for (const int ChildIndex : Node->GetChildrenNodeIndexList())
		{
			if (ChildIndex < 0 || ChildIndex >= NodeList.size()) continue;

			const auto& ChildNode = NodeList[ChildIndex];
			SetDrawable(Flag, ChildNode, NodeList);
		}
	}
}
#endif