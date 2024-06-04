#ifdef USE_GUIENGINE
#include "CGUIMaterialTab.h"
#include "../../Scene/CSceneController.h"
#include "../../Interface/IGraphicsAPI.h"
#include "../../Object/C3DObject.h"
#include <set>

namespace gui
{
	bool CGUIMaterialTab::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, const std::shared_ptr<scene::CSceneController>& SceneController, 
		int SelectedObjectIndex, int SelectedNodeIndex)
	{
		if (ImGui::BeginTabItem("Material"))
		{
			DrawMaterialGUI(pGraphicsAPI, ObjectList, SceneController, SelectedObjectIndex, SelectedNodeIndex);

			ImGui::EndTabItem();
		}

		return true;
	}

	bool CGUIMaterialTab::DrawMaterialGUI(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, const std::shared_ptr<scene::CSceneController>& SceneController, 
		int SelectedObjectIndex, int SelectedNodeIndex)
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
				if (ImGui::TreeNodeEx(Material->GetMaterialName().c_str(), ImGuiTreeNodeFlags_Framed))
				{
					//
					if (pGraphicsAPI->IsEnabledRuntimeShaderEditing())
					{
						const auto& CurrentMaterialFrame = Material->GetMaterialFrame();

						if (CurrentMaterialFrame)
						{
							if (ImGui::BeginCombo("MaterialFrame##CGUIMaterialTab", CurrentMaterialFrame->GetMaterialFrameName().c_str()))
							{
								const auto& MaterialFrameMap = SceneController->GetMaterialFrameMap();

								for (const auto& MaterialFrame : MaterialFrameMap)
								{
									const bool IsSelected = (CurrentMaterialFrame == MaterialFrame.second);

									std::string Label = MaterialFrame.first;
									if (ImGui::Selectable(Label.c_str(), IsSelected) && !IsSelected)
									{
										// マテリアルの置き換え
										auto NewMaterial = MaterialFrame.second->CreateMaterial(pGraphicsAPI, Material->GetRefCount(), Material->GetCullMode());
										Object->ReplaceMaterial(Material, NewMaterial);
									}
								}

								ImGui::EndCombo();
							}
						}
					}

					//
					auto& ShaderBufferList = Material->GetShaderBufferList();

					for (auto& UniformBuffer : ShaderBufferList)
					{
						const auto& BufferData = UniformBuffer->GetData();

						const auto& Descriptor = UniformBuffer->GetDescriptor();

						for (const auto& UniformDataMap : Descriptor->GetDataList())
						{
							const auto& UniformData = UniformDataMap.second;
							const auto ValueInput = UniformData.ValueInput;

							if (ValueInput.Hide) continue;

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

								graphics::EUniformInputType InputType = ValueInput.Type;

								switch (InputType)
								{
								case graphics::EUniformInputType::None:
									if (ImGui::InputFloat4(UniformName.c_str(), &val[0]))
									{
										Material->SetUniformValue(UniformName, &val, sizeof(val));
									}
									break;
								case graphics::EUniformInputType::Range:
									if (ImGui::SliderFloat4(UniformName.c_str(), &val[0], ValueInput.MinValue, ValueInput.MaxValue))
									{
										Material->SetUniformValue(UniformName, &val, sizeof(val));
									}
									break;
								case graphics::EUniformInputType::Color:
									{
										float col[4] = { val.x, val.y, val.z, val.w };
										if (ImGui::ColorEdit4(UniformName.c_str(), &col[0]))
										{
											Material->SetUniformValue(UniformName, &col[0], sizeof(float) * 4);
										}
									}
									break;
								default:
									break;
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

								graphics::EUniformInputType InputType = ValueInput.Type;

								switch (InputType)
								{
								case graphics::EUniformInputType::None:
									if (ImGui::InputFloat3(UniformName.c_str(), &val[0]))
									{
										Material->SetUniformValue(UniformName, &val, sizeof(val));
									}
									break;
								case graphics::EUniformInputType::Range:
									if (ImGui::SliderFloat3(UniformName.c_str(), &val[0], ValueInput.MinValue, ValueInput.MaxValue))
									{
										Material->SetUniformValue(UniformName, &val, sizeof(val));
									}
									break;
								case graphics::EUniformInputType::Color:
								{
									float col[3] = { val.x, val.y, val.z};
									if (ImGui::ColorEdit3(UniformName.c_str(), &col[0]))
									{
										Material->SetUniformValue(UniformName, &col[0], sizeof(float) * 3);
									}
								}
									break;
								default:
									break;
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

								graphics::EUniformInputType InputType = ValueInput.Type;

								switch (InputType)
								{
								case graphics::EUniformInputType::None:
									if (ImGui::InputFloat2(UniformName.c_str(), &val[0]))
									{
										Material->SetUniformValue(UniformName, &val, sizeof(val));
									}
									break;
								case graphics::EUniformInputType::Range:
									if (ImGui::SliderFloat2(UniformName.c_str(), &val[0], ValueInput.MinValue, ValueInput.MaxValue))
									{
										Material->SetUniformValue(UniformName, &val, sizeof(val));
									}
									break;
								case graphics::EUniformInputType::Color:
									break;
								default:
									break;
								}
							}
							break;
							case graphics::EUniformValueType::VALUE_TYPE_FLOAT:
							{
								const std::string& UniformName = UniformData.UniformName;
								float val = GetFloat(BufferData, UniformData.ByteOffset);

								graphics::EUniformInputType InputType = ValueInput.Type;

								switch (InputType)
								{
								case graphics::EUniformInputType::None:
									if (ImGui::InputFloat(UniformName.c_str(), &val))
									{
										Material->SetUniformValue(UniformName, &val, sizeof(val));
									}
									break;
								case graphics::EUniformInputType::Range:
									if (ImGui::SliderFloat(UniformName.c_str(), &val, ValueInput.MinValue, ValueInput.MaxValue))
									{
										Material->SetUniformValue(UniformName, &val, sizeof(val));
									}
									break;
								case graphics::EUniformInputType::Color:
									break;
								default:
									break;
								}
							}
							break;
							case graphics::EUniformValueType::VALUE_TYPE_INT:
							{
								const std::string& UniformName = UniformData.UniformName;
								int val = GetInt(BufferData, UniformData.ByteOffset);

								graphics::EUniformInputType InputType = ValueInput.Type;

								switch (InputType)
								{
								case graphics::EUniformInputType::None:
									if (ImGui::InputInt(UniformName.c_str(), &val))
									{
										Material->SetUniformValue(UniformName, &val, sizeof(val));
									}
									break;
								case graphics::EUniformInputType::Range:
									if (ImGui::SliderInt(UniformName.c_str(), &val, static_cast<int>(ValueInput.MinValue), static_cast<int>(ValueInput.MaxValue)))
									{
										Material->SetUniformValue(UniformName, &val, sizeof(val));
									}
									break;
								case graphics::EUniformInputType::Color:
									break;
								default:
									break;
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