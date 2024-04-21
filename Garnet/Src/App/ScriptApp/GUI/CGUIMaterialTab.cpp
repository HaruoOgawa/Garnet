#ifdef USE_GUIENGINE
#include "CGUIMaterialTab.h"
#include "../../Object/C3DObject.h"
#include <set>

namespace gui
{
	bool CGUIMaterialTab::Draw(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, int SelectedObjectIndex, int SelectedNodeIndex)
	{
		if (ImGui::BeginTabItem("Material"))
		{
			DrawMaterialGUI(ObjectList, SelectedObjectIndex, SelectedNodeIndex);

			ImGui::EndTabItem();
		}

		return true;
	}

	bool CGUIMaterialTab::DrawMaterialGUI(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, int SelectedObjectIndex, int SelectedNodeIndex)
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

		for (int MaterialIndex : MaterialIndexSet)
		{
			if (MaterialIndex >= 0 && MaterialIndex < static_cast<int>(MaterialList.size()))
			{
				const auto& Material = MaterialList[MaterialIndex];

				// マテリアル名
				if (ImGui::TreeNodeEx(Material->GetMaterialName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow))
				{
					auto& ShaderBufferList = Material->GetShaderBufferList();

					for (auto& UniformBuffer : ShaderBufferList)
					{
						const auto& BufferData = UniformBuffer->GetData();

						const auto& Descriptor = UniformBuffer->GetDescriptor();

						for (const auto& UniformDataMap : Descriptor->GetDataList())
						{
							const auto& UniformData = UniformDataMap.second;

							switch (UniformData.ValueType)
							{
							case graphics::EUniformValueType::NONE:
								continue;
							case graphics::EUniformValueType::VALUE_TYPE_MAT4:
								continue;
							case graphics::EUniformValueType::VALUE_TYPE_MAT3:
								continue;
							case graphics::EUniformValueType::VALUE_TYPE_MAT2:
								continue;
							case graphics::EUniformValueType::VALUE_TYPE_VEC4:
							{
								const std::string& UniformName = UniformData.UniformName;
								glm::vec4 val = glm::vec4(
									GetFloat(BufferData, UniformData.ByteOffset + sizeof(float) * 0),
									GetFloat(BufferData, UniformData.ByteOffset + sizeof(float) * 1),
									GetFloat(BufferData, UniformData.ByteOffset + sizeof(float) * 2),
									GetFloat(BufferData, UniformData.ByteOffset + sizeof(float) * 3)
								);

								if (ImGui::InputFloat4(UniformName.c_str(), &val[0]))
								{
									Material->SetUniformValue(UniformName, &val, sizeof(val));
								}
							}
							break;
							case graphics::EUniformValueType::VALUE_TYPE_VEC3:
							{
								const std::string& UniformName = UniformData.UniformName;
								glm::vec3 val = glm::vec3(
									GetFloat(BufferData, UniformData.ByteOffset + sizeof(float) * 0),
									GetFloat(BufferData, UniformData.ByteOffset + sizeof(float) * 1),
									GetFloat(BufferData, UniformData.ByteOffset + sizeof(float) * 2)
								);

								if (ImGui::InputFloat3(UniformName.c_str(), &val[0]))
								{
									Material->SetUniformValue(UniformName, &val, sizeof(val));
								}
							}
							break;
							case graphics::EUniformValueType::VALUE_TYPE_VEC2:
							{
								const std::string& UniformName = UniformData.UniformName;
								glm::vec2 val = glm::vec2(
									GetFloat(BufferData, UniformData.ByteOffset + sizeof(float) * 0),
									GetFloat(BufferData, UniformData.ByteOffset + sizeof(float) * 1)
								);

								if (ImGui::InputFloat2(UniformName.c_str(), &val[0]))
								{
									Material->SetUniformValue(UniformName, &val, sizeof(val));
								}
							}
							break;
							case graphics::EUniformValueType::VALUE_TYPE_FLOAT:
							{
								const std::string& UniformName = UniformData.UniformName;
								float val = GetFloat(BufferData, UniformData.ByteOffset);

								if (ImGui::InputFloat(UniformName.c_str(), &val))
								{
									Material->SetUniformValue(UniformName, &val, sizeof(val));
								}
							}
							break;
							case graphics::EUniformValueType::VALUE_TYPE_INT:
							{
								const std::string& UniformName = UniformData.UniformName;
								int val = GetInt(BufferData, UniformData.ByteOffset);

								if (ImGui::InputInt(UniformName.c_str(), &val))
								{
									Material->SetUniformValue(UniformName, &val, sizeof(val));
								}
							}
							break;
							case graphics::EUniformValueType::VALUE_TYPE_FLOAT_ARRAY:
								continue;
							case graphics::EUniformValueType::VALUE_TYPE_MAT4_ARRAY:
								continue;
							default:
								break;
							}
						}
					}

					ImGui::TreePop();
				}
			}
		}

		return true;
	}

	float CGUIMaterialTab::GetFloat(const std::vector<unsigned char>& BufferData, int Offset)
	{
		const unsigned char* CurrPointer = &BufferData[Offset];

		auto val = (CurrPointer[3] << 24) | (CurrPointer[2] << 16) | (CurrPointer[1] << 8) | (CurrPointer[0]);

		float Dst = *reinterpret_cast<const float*>(&val);

		return Dst;
	}
	
	int CGUIMaterialTab::GetInt(const std::vector<unsigned char>& BufferData, int Offset)
	{
		const unsigned char* CurrPointer = &BufferData[Offset];

		auto val = (CurrPointer[3] << 24) | (CurrPointer[2] << 16) | (CurrPointer[1] << 8) | (CurrPointer[0]);

		int Dst = *reinterpret_cast<const int*>(&val);

		return Dst;
	}
}
#endif