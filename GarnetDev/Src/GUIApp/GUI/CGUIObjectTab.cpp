#ifdef USE_GUIENGINE
#include "CGUIObjectTab.h"
#include "CGUIMaterialTab.h"
#include "CGUITransformTab.h"

#include <Scene/CSceneController.h>
#include <Object/C3DObject.h>
#include <Message/Console.h>
#include <LoadWorker/CLoadWorker.h>

namespace gui
{
	CGUIObjectTab::CGUIObjectTab():
		m_SelectedObjectIndex(-1),
		m_SelectedNodeIndex(-1),
		m_SelectedName(""),
		m_OperateButtonID(-1),
		m_ShowAddObjectDialog(false)
	{
	}

	void CGUIObjectTab::Reset()
	{
		m_OperateButtonID = -1;
	}

	bool CGUIObjectTab::Draw(api::IGraphicsAPI* pGraphicsAPI, const SGUIParams& GUIParams)
	{
		Reset();

		if (ImGui::BeginTabItem("Object"))
		{
			
			if (!DrawObjectList(pGraphicsAPI, GUIParams)) return false;
			if (!DrawObjectDetail(pGraphicsAPI, GUIParams)) return false;

			ImGui::EndTabItem();
		}

		if (m_ShowAddObjectDialog)
		{
			if (!DrawAddObjectDialog(pGraphicsAPI, GUIParams)) return false;
		}

		return true;
	}

	bool CGUIObjectTab::DrawObjectList(api::IGraphicsAPI* pGraphicsAPI, const SGUIParams& GUIParams)
	{
		if (ImGui::BeginChild("ObjectListChild", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.25f), ImGuiChildFlags_Border, 0))
		{
			for (int CurrentObjectIndex = 0; CurrentObjectIndex < static_cast<int>(GUIParams.ObjectList.size()); CurrentObjectIndex++)
			{
				const auto& Object = GUIParams.ObjectList[CurrentObjectIndex];

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

				if (IsOpend)
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

			if (ImGui::Button("AddObject##ObjectList"))
			{
				m_ShowAddObjectDialog = true;
			}

			ImGui::EndChild();
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

	bool CGUIObjectTab::DrawObjectDetail(api::IGraphicsAPI* pGraphicsAPI, const SGUIParams& GUIParams)
	{
		// ImGui::GetContentRegionAvail().y * 0.5とかの後に0にしたらなんかしらんがいい感じにぴったりの位置に配置してくれる
		if (ImGui::BeginChild("ObjectDetailChild", ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Border, 0))
		{
			if (!m_SelectedName.empty())
			{
				ImGui::Text("%s", m_SelectedName.c_str());
			}

			if (ImGui::BeginTabBar("ObjectDetail"))
			{
				if (!CGUITransformTab::Draw(GUIParams.ObjectList, m_SelectedObjectIndex, m_SelectedNodeIndex)) return false;
				if (!CGUIMaterialTab::Draw(pGraphicsAPI, GUIParams.ObjectList, GUIParams.SceneController, m_SelectedObjectIndex, m_SelectedNodeIndex)) return false;

				ImGui::EndTabBar(); // ObjectDetail
			}

			ImGui::EndChild();
		}

		return true;
	}

	bool CGUIObjectTab::DrawAddObjectDialog(api::IGraphicsAPI* pGraphicsAPI, const SGUIParams& GUIParams)
	{
		ImGuiIO& io = ImGui::GetIO();

		ImVec2 WindowSize = ImVec2(io.DisplaySize.x * 0.15f, io.DisplaySize.y * 0.15f);

		ImGui::SetNextWindowPos(ImVec2(io.MousePos.x - WindowSize.x * 0.5f, io.MousePos.y - WindowSize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(WindowSize, ImGuiCond_Appearing);

		if (ImGui::Begin("AddObject##ObjectTab", &m_ShowAddObjectDialog))
		{
			ImGui::Text("Basic Info");
			ImGui::SameLine();
			ImGui::Separator();

			// ObjectName
			static std::string ObjectName = std::string();
			{
				static char buf[256] = "";

				if (ImGui::InputText("ObjectName##AddObjectDialog", buf, IM_ARRAYSIZE(buf)))
				{
					ObjectName = std::string(buf);
				}
			}

			// PassName
			static std::string PassName = std::string();
			{
				static char buf[256] = "";

				if (ImGui::InputText("PassName##AddObjectDialog", buf, IM_ARRAYSIZE(buf)))
				{
					PassName = std::string(buf);
				}
			}

			// DepthPassName
			static std::string DepthPassName = std::string();
			{
				static char buf[256] = "";

				if (ImGui::InputText("DepthPassName##AddObjectDialog", buf, IM_ARRAYSIZE(buf)))
				{
					DepthPassName = std::string(buf);
				}
			}

			// DefaultMaterialframe
			static std::string DefaultMaterialframe = std::string();
			{
				static char buf[256] = "";

				if (ImGui::InputText("DefaultMaterialframe##AddObjectDialog", buf, IM_ARRAYSIZE(buf)))
				{
					DefaultMaterialframe = std::string(buf);
				}
			}

			ImGui::Text("Add");
			ImGui::SameLine();
			ImGui::Separator();

			// AddEmpty
			if (ImGui::Button("AddEmpty##ObjectTab") && !ObjectName.empty())
			{
				m_ShowAddObjectDialog = false;

				// Object作成
				std::shared_ptr<object::C3DObject> Object = std::make_shared<object::C3DObject>("", "");
				Object->SetObjectName(ObjectName);
				Object->SetPassName(PassName);
				Object->SetDepthPassName(DepthPassName);

				GUIParams.SceneController->AddObject(Object);
			}

			// AddFile
			{
				
				static std::string fileName = std::string();

				if (ImGui::Button("AddFile##ObjectTab") && !ObjectName.empty() && !fileName.empty())
				{
					m_ShowAddObjectDialog = false;

					// Object作成
					std::shared_ptr<object::C3DObject> Object = std::make_shared<object::C3DObject>("", "");
					Object->SetObjectName(ObjectName);
					Object->SetPassName(PassName);
					Object->SetDepthPassName(DepthPassName);

					GUIParams.SceneController->AddObjectWithLoading(GUIParams.pLoadWorker, Object, fileName, DefaultMaterialframe);
				}

				ImGui::SameLine();

				// fileName
				{
					static char buf[256] = "";
					if (ImGui::InputText("##AddObjectDialog_FileName", buf, IM_ARRAYSIZE(buf)))
					{
						fileName = std::string(buf);
					}
				}
			}
		}

		ImGui::End();

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