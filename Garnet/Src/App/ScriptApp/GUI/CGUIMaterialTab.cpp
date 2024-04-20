#ifdef USE_GUIENGINE
#include "CGUIMaterialTab.h"
#include "../../Object/C3DObject.h"
#include <set>

namespace gui
{
	bool CGUIMaterialTab::Draw(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, int SelectedObjectIndex, int SelectedNodeIndex)
	{
		if (SelectedObjectIndex == -1 || SelectedNodeIndex == -1) return true;
		
		// Objectを取得
		if (SelectedObjectIndex < 0 || SelectedObjectIndex >= static_cast<int>(ObjectList.size())) return true;
		
		const auto& Object = ObjectList[SelectedObjectIndex];

		// Nodeを取得
		const auto& NodeList = Object->GetNodeList();
		if (SelectedNodeIndex < 0 || SelectedNodeIndex >= static_cast<int>(NodeList.size())) return true;

		const auto& Node = NodeList[SelectedNodeIndex];

		// Meshを取得
		const auto& MeshList = Object->GetMeshList();
		int MeshIndex = Node->GetMeshIndex();
		if (MeshIndex < 0 || MeshIndex >= static_cast<int>(MeshList.size())) return true;

		const auto& Mesh = MeshList[MeshIndex];

		// Mesh - Primitiveから使用するMaterialのIndexSetを取得
		std::set<int> MaterialIndexSet;

		for (const auto& Primitive : Mesh->GetPrimitiveList())
		{
			MaterialIndexSet.emplace(Primitive->GetMaterialIndex());
		}

		// MaterialのGUIを描画
		const auto& MaterialList = Object->GetMaterialList();

		if (ImGui::BeginTabItem("Material"))
		{
			ImGui::Text("This is Material Window");

			for (int MaterialIndex : MaterialIndexSet)
			{
				if (MaterialIndex >= 0 && MaterialIndex < static_cast<int>(MaterialList.size()))
				{
					const auto& Material = MaterialList[MaterialIndex];

					// マテリアル名
					if (ImGui::TreeNodeEx(Material->GetMaterialName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow))
					{
						ImGui::Text("Shader Value");

						auto& ShaderBufferList = Material->GetShaderBufferList();

						for (auto& UniformBuffer : ShaderBufferList)
						{
							const auto& BufferData = UniformBuffer->GetData();

							const auto& Descriptor = UniformBuffer->GetDescriptor();

							for (const auto& UniformDataMap : Descriptor->GetDataList())
							{
								const auto& UniformData = UniformDataMap.second;
							}
						}

						ImGui::TreePop();
					}
				}
			}

			ImGui::EndTabItem();
		}

		return true;
	}
}
#endif