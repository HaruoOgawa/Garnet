#ifdef USE_BINARY_WRITE
#include "CSceneWriter.h"
#include "CSceneController.h"
#include "../Object/C3DObject.h"
#include "../Animation/CAnimationClipSet.h"
#include "../Audio/CAudioClip.h"
#include "../LoadWorker/CFile.h"
#include "../Timeline/CTimelineController.h"
#include "../Interface/IPhysicsEngine.h"
#include "../Graphics/CVertexBuffer.h"

namespace scene
{
	bool CSceneWriter::Write(CSceneController* pSceneController, const std::shared_ptr<timeline::CTimelineController>& TimelineController, physics::IPhysicsEngine* pPhysicsEngine)
	{
		ordered_json SceneJSON;

		// シーンJSONに現在の状態を書き出し
		if (!WriteScene(SceneJSON, pSceneController, TimelineController, pPhysicsEngine)) return false;

		// データを取得
		const auto& JSONStr = SceneJSON.dump(4);

		std::vector<unsigned char> Data;
		Data.resize(JSONStr.size());

		std::memcpy(&Data[0], &JSONStr[0], static_cast<int>(JSONStr.size()));

		// ファイル書き出し
		resource::CFile File = resource::CFile(pSceneController->GetFileName());
		File.SetData(Data);
		if (!File.Write()) return false;

		return true;
	}

	bool CSceneWriter::WriteScene(ordered_json& SceneJSON, CSceneController* pSceneController, const std::shared_ptr<timeline::CTimelineController>& TimelineController, physics::IPhysicsEngine* pPhysicsEngine)
	{
		if (!WriteMaterialFrames(SceneJSON, pSceneController)) return false;
		if (!WriteValueRegistries(SceneJSON, pSceneController, TimelineController)) return false;
		if (!WriteSceneTextureSet(SceneJSON, pSceneController)) return false;
		if (!WriteAnimations(SceneJSON, pSceneController)) return false;
		if (!WriteSound(SceneJSON, pSceneController)) return false;
		if (!WriteTimeline(SceneJSON, pSceneController)) return false;
		if (!WritePhysics(SceneJSON, pPhysicsEngine)) return false;
		if (!WriteObjects(SceneJSON, pSceneController, TimelineController)) return false;

		return true;
	}

	bool CSceneWriter::WriteMaterialFrames(ordered_json& SceneJSON, CSceneController* pSceneController)
	{
		const auto& MaterialFrameMap = pSceneController->GetMaterialFrameMap();

		for (const auto& MaterialFrame : MaterialFrameMap)
		{
			SceneJSON["materialframes"].push_back({ {"name" , MaterialFrame.second->GetMaterialFrameName()}, {"filename", MaterialFrame.second->GetFileName()}});
		}

		return true;
	}

	bool CSceneWriter::WriteValueRegistries(ordered_json& SceneJSON, CSceneController* pSceneController, const std::shared_ptr<timeline::CTimelineController>& TimelineController)
	{
		// Scene
		for (const auto& ValueRegistry : pSceneController->GetValueRegistryList())
		{
			if (!WriteValueRegistry(SceneJSON, ValueRegistry, TimelineController)) return false;
		}

		
		for (const auto& Object : pSceneController->GetObjectList())
		{
			// Object
			for (const auto& Component : Object->GetComponentList())
			{
				if (!WriteValueRegistry(SceneJSON, std::make_pair(Component->GetRegistryName(), Component->GetValueRegistry()), TimelineController)) return false;
			}

			// Node
			for (const auto& Node : Object->GetNodeList())
			{
				for (const auto& Component : Node->GetComponentList())
				{
					if (!WriteValueRegistry(SceneJSON, std::make_pair(Component->GetRegistryName(), Component->GetValueRegistry()), TimelineController)) return false;
				}
			}
		}

		return true;
	}

	bool CSceneWriter::WriteValueRegistry(ordered_json& SceneJSON, const std::pair<std::string, std::shared_ptr<scriptable::CValueRegistry>>& ValueRegistry,
		const std::shared_ptr<timeline::CTimelineController>& TimelineController)
	{
		// 使用しているトラックの種類
		std::set<std::string> TrackValueNameList;
		if (TimelineController)
		{
			const auto& Clip = TimelineController->GetClip();

			if (Clip)
			{
				for (const auto& TrackID : ValueRegistry.second->GetRefTrackIDList())
				{
					auto Track = Clip->FindTrack(TrackID);
					if (!Track) continue;

					TrackValueNameList.emplace(Track->GetParam_String("ValueName"));
				}
			}
		}

		ordered_json ValueRegistryJSON;

		ValueRegistryJSON["registryname"] = ValueRegistry.first;

		// Timeline Track ID
		for (const auto& TrackID : ValueRegistry.second->GetRefTrackIDList())
		{
			ValueRegistryJSON["trackids"].push_back(TrackID);
		}

		for (const auto& Value : ValueRegistry.second->GetValueList())
		{
			// タイムラインで管理されていたら書き込まない
			//if (TrackValueNameList.find(Value.second.Name) != TrackValueNameList.end()) continue;

			ordered_json ValueJSON;

			ValueJSON["name"] = Value.second.Name;

			// type
			std::string type = std::string();
			graphics::EUniformValueType ValueType = Value.second.Type;
			switch (ValueType)
			{
			case graphics::EUniformValueType::NONE:
				break;
			case graphics::EUniformValueType::VALUE_TYPE_MAT4:
				type = "mat4";
				break;
			case graphics::EUniformValueType::VALUE_TYPE_MAT3:
				type = "mat3";
				break;
			case graphics::EUniformValueType::VALUE_TYPE_MAT2:
				type = "mat2";
				break;
			case graphics::EUniformValueType::VALUE_TYPE_VEC4:
				type = "vec4";
				break;
			case graphics::EUniformValueType::VALUE_TYPE_VEC3:
				type = "vec3";
				break;
			case graphics::EUniformValueType::VALUE_TYPE_VEC2:
				type = "vec2";
				break;
			case graphics::EUniformValueType::VALUE_TYPE_FLOAT:
				type = "float";
				break;
			case graphics::EUniformValueType::VALUE_TYPE_INT:
				type = "int";
				break;
			case graphics::EUniformValueType::VALUE_TYPE_STRING:
				type = "string";
				break;
			case graphics::EUniformValueType::VALUE_TYPE_FLOAT_ARRAY:
				break;
			case graphics::EUniformValueType::VALUE_TYPE_MAT4_ARRAY:
				break;
			default:
				break;
			}

			if (type.empty()) continue;

			ValueJSON["type"] = type;

			ordered_json valueJSON;

			// Value
			if (type == "int")
			{
				std::vector<int> DstValue(Value.second.ByteSize / 4);
				std::memcpy(&DstValue[0], &Value.second.Buffer[0], Value.second.ByteSize);

				for (auto v : DstValue)
				{
					ValueJSON["initValue"].push_back(v);
				}
			}
			else if (type == "string")
			{
				std::string DstValue = std::string();
				DstValue.resize(Value.second.ByteSize);

				std::memcpy(&DstValue[0], &Value.second.Buffer[0], Value.second.ByteSize);

				ValueJSON["initValue"] = DstValue;
			}
			else
			{
				std::vector<float> DstValue(Value.second.ByteSize / 4);
				std::memcpy(&DstValue[0], &Value.second.Buffer[0], Value.second.ByteSize);

				for (auto v : DstValue)
				{
					ValueJSON["initValue"].push_back(v);
				}
			}

			//
			ValueRegistryJSON["values"].push_back(ValueJSON);
		}

		SceneJSON["valueregistries"].push_back(ValueRegistryJSON);

		return true;
	}

	bool CSceneWriter::WriteSceneTextureSet(ordered_json& SceneJSON, CSceneController* pSceneController)
	{
		const auto& SceneTextureSet = pSceneController->GetSceneTextureSet();
		if (!SceneTextureSet) return true;

		for (const auto& CubeMap : SceneTextureSet->GetCubeMapList())
		{
			const auto& FileNameList = CubeMap->GetFileNameList();
			if (FileNameList.size() != 6) continue;

			SceneJSON["scenetextureset"]["cubemaps"].push_back({
				{"back", FileNameList[0]},
				{"bottom", FileNameList[1]},
				{"front", FileNameList[2]},
				{"left", FileNameList[3]},
				{"right", FileNameList[4]},
				{"top", FileNameList[5]}
			});
		}

		SceneJSON["scenetextureset"]["frametextures"] = {};

		const auto& Diffuse_Tex = SceneTextureSet->GetDiffuse_Tex();
		const auto& Specular_Tex = SceneTextureSet->GetSpecular_Tex();
		const auto& GGXLUT_Tex = SceneTextureSet->GetGGXLUT_Tex();
		if (Diffuse_Tex && Specular_Tex && GGXLUT_Tex)
		{
			SceneJSON["scenetextureset"]["ibl"] = {
				{"diffuse", Diffuse_Tex->GetFileName()},
				{"specular", Specular_Tex->GetFileName()},
				{"ggx", GGXLUT_Tex->GetFileName()}
			};
		}

		return true;
	}

	bool CSceneWriter::WriteAnimations(ordered_json& SceneJSON, CSceneController* pSceneController)
	{
		const auto& AnimationClipSetMap = pSceneController->GetAnimationClipSetMap();

		for (const auto& AnimationClipSet : AnimationClipSetMap)
		{
			json animation{};

			const auto& Clip = AnimationClipSet.second->GetAnimationClip(0);
			if (Clip)
			{
				const auto& Skeleton = Clip->GetDefaultSkeleton();
				if (Skeleton)
				{
					switch (Skeleton->GetRig())
					{
					case animation::ERigType::Humanoid:
						animation["rig"] = "humanoid";
						break;
					default:
						break;
					}

					// HumanoidBoneList
					WriteHumanoidBoneList(animation, Skeleton);
				}
			}

			animation["name"] = AnimationClipSet.first;
			animation["filename"] = AnimationClipSet.second->GetFileName();

			SceneJSON["animations"].emplace_back(animation);
		}

		return true;
	}

	bool CSceneWriter::WriteSound(ordered_json& SceneJSON, CSceneController* pSceneController)
	{
		const auto& Sound = pSceneController->GetSound();

		const auto& AudioClip = std::get<0>(Sound);
		bool autoplay = std::get<1>(Sound);
		bool loop = std::get<2>(Sound);
		
		if (AudioClip)
		{
			SceneJSON["sound"]["bgm"] = {
				{ "filename", AudioClip->GetFileName() },
				{ "autoplay", autoplay },
				{ "loop", loop }
			};
		}

		return true;
	}

	bool CSceneWriter::WriteTimeline(ordered_json& SceneJSON, CSceneController* pSceneController)
	{
		SceneJSON["timeline"]["filename"] = pSceneController->GetTimelineFileName();

		return true;
	}

	bool CSceneWriter::WritePhysics(ordered_json& SceneJSON, physics::IPhysicsEngine* pPhysicsEngine)
	{
		if (pPhysicsEngine)
		{
			SceneJSON["physics"]["enabled"] = pPhysicsEngine->IsEnabled();
		}

		return true;
	}

	bool CSceneWriter::WriteObjects(ordered_json& SceneJSON, CSceneController* pSceneController, const std::shared_ptr<timeline::CTimelineController>& TimelineController)
	{
		const auto& ObjectList = pSceneController->GetObjectList();

		for (const auto& Object : ObjectList)
		{
			const bool StoredFile3DModel = !Object->GetFileName().empty();

			std::map<std::string, std::shared_ptr<graphics::CTexture>> TextureInfoList;
			{
				const auto& TextureInfoMap = pSceneController->GetTextureInfoMap();
				const auto& it = TextureInfoMap.find(Object);
				if (it != TextureInfoMap.end())
				{
					TextureInfoList = it->second;
				}
			}

			ordered_json ObjectJSON;

			ObjectJSON["name"] = Object->GetObjectName();
			ObjectJSON["filename"] = Object->GetFileName();

			if (StoredFile3DModel)
			{
				for (const auto& defaultmaterialframe : Object->GetDefaultMaterialFrameList())
				{
					ObjectJSON["defaultmaterialframes"].push_back(defaultmaterialframe);
				}
			}

			int InstanceCount = 1;
			for (const auto& Mesh : Object->GetMeshList())
			{
				for (const auto& VertexBuffer : Mesh->GetVertexBufferList())
				{
					InstanceCount = std::max(InstanceCount, VertexBuffer->GetInstanceCount());
				}
			}

			if(InstanceCount > 1) ObjectJSON["instancecount"] = InstanceCount;

			for (const auto& renderpass : Object->GetPassNameList())
			{
				ObjectJSON["renderpasslist"].push_back(renderpass);
			}

			// animation
			if (StoredFile3DModel)
			{
				const auto& AnimationInfoMap = pSceneController->GetAnimationInfoMap();
				const auto& it = AnimationInfoMap.find(Object);

				if (it != AnimationInfoMap.end())
				{
					if (!WriteAnimation(ObjectJSON, Object.get(), it->second)) return false;
				}
			}

			// transform
			{
				const auto& transform = Object->GetObjectTransform();
				const auto& rotate = glm::eulerAngles(transform->GetRot());
				
				ObjectJSON["transform"] = {
					{ "pos", {transform->GetPos().x, transform->GetPos().y, transform->GetPos().z} },
					{ "rotate", {glm::degrees(rotate.x), glm::degrees(rotate.y), glm::degrees(rotate.z)} },
					{ "scale", {transform->GetScale().x, transform->GetScale().y, transform->GetScale().z} }
				};
			}

			// enable
			ObjectJSON["enable"] = Object->IsEnabled();

			// rootnodes
			if (!StoredFile3DModel)
			{
				for (int RootNodeIndex : Object->GetRootNodeIndexList())
				{
					ObjectJSON["rootnodes"].push_back(RootNodeIndex);
				}
			}

			// nodes
			if (!StoredFile3DModel)
			{
				if (!WriteNodes(ObjectJSON, Object.get(), TimelineController)) return false;
			}

			// meshs
			if (!StoredFile3DModel)
			{
				if (!WriteMeshs(ObjectJSON, Object.get())) return false;
			}

			// materials
			{
				if (!WriteMaterials(ObjectJSON, Object.get(), TextureInfoList, TimelineController)) return false;
			}

			// textureset
			{
				if (!WriteTextureSet(ObjectJSON, Object.get(), TextureInfoList)) return false;
			}

			// joints
			{
				if (!WriteJoints(ObjectJSON, Object.get())) return false;
			}

			// コンポーネント
			for (const auto& Component : Object->GetComponentList())
			{
				ordered_json componentJSON;
				componentJSON["type"] = Component->GetComponentName();
				if (!Component->GetRegistryName().empty()) componentJSON["valueregistry"] = Component->GetRegistryName();

				ObjectJSON["components"].push_back(componentJSON);
			}

			// objectsに追加
			SceneJSON["objects"].push_back(ObjectJSON);
		}

		return true;
	}

	bool CSceneWriter::WriteNodes(ordered_json& ObjectJSON, object::C3DObject* pObject, const std::shared_ptr<timeline::CTimelineController>& TimelineController)
	{
		const auto& NodeList = pObject->GetNodeList();

		for (const auto& Node : NodeList)
		{
			// 使用しているトラックの種類
			std::set<std::string> TrackNameList;
			if (TimelineController)
			{
				const auto& Clip = TimelineController->GetClip();

				if (Clip)
				{
					for (const auto& TrackID : Node->GetRefTrackIDList())
					{
						auto Track = Clip->FindTrack(TrackID);
						if (!Track) continue;

						TrackNameList.emplace(Track->GetTrackName());
					}
				}
			}

			//
			ordered_json node;

			node["name"] = Node->GetName();

			// transform
			{
				const auto& transform = Node->GetLocalTransform();
				const auto& rotate = glm::eulerAngles(transform->GetRot());

				if(TrackNameList.find("Translation") == TrackNameList.end()) node["transform"]["pos"] = { transform->GetPos().x, transform->GetPos().y, transform->GetPos().z };
				if (TrackNameList.find("Rotation") == TrackNameList.end()) node["transform"]["rotate"] = { glm::degrees(rotate.x), glm::degrees(rotate.y), glm::degrees(rotate.z) };
				if (TrackNameList.find("Scale") == TrackNameList.end()) node["transform"]["scale"] = { transform->GetScale().x, transform->GetScale().y, transform->GetScale().z };
			}

			// enable
			if (TrackNameList.find("EnabledFlag") == TrackNameList.end()) node["enable"] = Node->IsEnabled();

			// 物理演算
			if (!Node->GetPhysicsObjectList().empty())
			{
				const auto& PhysicsObject = Node->GetPhysicsObjectList()[0];
				const auto& RBParam = PhysicsObject->GetRbParam();
				
				std::string PhysicsTypeStr = std::string();
				switch (RBParam.PhysicsType)
				{
				case physics::EPhysicsType::STATIC:
					PhysicsTypeStr = "static";
					break;

				case physics::EPhysicsType::DYNAMIC:
					PhysicsTypeStr = "dynamic";
					break;

				case physics::EPhysicsType::DYNAMIC_JOINT:
					PhysicsTypeStr = "dynamic_joint";
					break;
				default:
					break;
				}

				// Collider(物理用途以外のコライダーが増えたら分離するかも)
				{
					std::string PhysicsShapeStr = std::string();
					switch (PhysicsObject->GetPhysicsShape())
					{
					case physics::EPhysicsShape::BOX:
						PhysicsShapeStr = "box";
						break;

					case physics::EPhysicsShape::SPHERE:
						PhysicsShapeStr = "sphere";
						break;

					case physics::EPhysicsShape::CAPSULE:
						PhysicsShapeStr = "capsule";
						break;

					default:
						break;
					}

					const auto& Size = PhysicsObject->GetSize();

					node["collider"]["shape"] = PhysicsShapeStr;
					node["collider"]["size"] = { Size.x, Size.y, Size.z };
				}

				// RigidBody
				node["rigidbody"]["mass"] = RBParam.Mass;
				node["rigidbody"]["physicstype"] = PhysicsTypeStr;
				node["rigidbody"]["transdamping"] = RBParam.TransDamping;
				node["rigidbody"]["rotatedamping"] = RBParam.RotateDamping;
				node["rigidbody"]["repulsion"] = RBParam.Repulsion;
				node["rigidbody"]["friction"] = RBParam.Friction;
			}

			//
			node["meshindex"] = Node->GetMeshIndex();

			//
			for (int ChildIndex : Node->GetChildrenNodeIndexList())
			{
				node["children"].push_back(ChildIndex);
			}

			// Timeline Track ID
			for (const auto& TrackID : Node->GetRefTrackIDList())
			{
				node["trackids"].push_back(TrackID);
			}

			// コンポーネント
			for (const auto& Component : Node->GetComponentList())
			{
				ordered_json componentJSON;
				componentJSON["type"] = Component->GetComponentName();
				if(!Component->GetRegistryName().empty()) componentJSON["valueregistry"] = Component->GetRegistryName();

				node["components"].push_back(componentJSON);
			}

			ObjectJSON["nodes"].push_back(node);
		}

		return true;
	}

	bool CSceneWriter::WriteMeshs(ordered_json& ObjectJSON, object::C3DObject* pObject)
	{
		const auto& MeshList = pObject->GetMeshList();

		for (const auto& Mesh : MeshList)
		{
			ordered_json meshJSON;

			const auto& PrimitiveList = Mesh->GetPrimitiveList();

			for (int PrimitiveIndex = 0; PrimitiveIndex < static_cast<int>(PrimitiveList.size()); PrimitiveIndex++)
			{
				const auto& Primitive = PrimitiveList[PrimitiveIndex];

				ordered_json primitiveJSON;

				graphics::EPresetPrimitiveType PresetType = Primitive->GetPresetType();
				std::string type = std::string();

				switch (PresetType)
				{
				case graphics::EPresetPrimitiveType::None:
					break;
				case graphics::EPresetPrimitiveType::BOARD:
					type = "board";
					break;
				case graphics::EPresetPrimitiveType::CUBE:
					type = "cube";
					break;
				case graphics::EPresetPrimitiveType::POINT:
					type = "point";
					break;
				case graphics::EPresetPrimitiveType::SPHERE:
					type = "sphere";
					break;
				default:
					break;
				}

				primitiveJSON["type"] = type;

				meshJSON["primitives"].push_back(primitiveJSON);
			}

			ObjectJSON["meshs"].push_back(meshJSON);
		}

		return true;
	}

	bool CSceneWriter::WriteMaterials(ordered_json& ObjectJSON, object::C3DObject* pObject, const std::map<std::string, std::shared_ptr<graphics::CTexture>>& TextureInfoList, const std::shared_ptr<timeline::CTimelineController>& TimelineController)
	{
		const auto& TextureSet = pObject->GetTextureSet();
		const auto& Texture2DList = TextureSet->Get2DTextureList();

		const auto& MeshList = pObject->GetMeshList();

		for (int MeshIndex = 0; MeshIndex < static_cast<int>(MeshList.size()); MeshIndex++)
		{
			const auto& Mesh = MeshList[MeshIndex];

			const auto& PrimitiveList = Mesh->GetPrimitiveList();

			for (int PrimitiveIndex = 0; PrimitiveIndex < static_cast<int>(PrimitiveList.size()); PrimitiveIndex++)
			{
				const auto& Primitive = PrimitiveList[PrimitiveIndex];

				for (const auto& Renderer : Primitive->GetRendererList())
				{
					const auto& Material = std::get<1>(Renderer);
					if (!Material) continue;

					// 使用しているトラックの種類
					std::set<std::string> TrackUniformNameList;
					if (TimelineController)
					{
						const auto& Clip = TimelineController->GetClip();

						if (Clip)
						{
							for (const auto& TrackID : Material->GetRefTrackIDList())
							{
								auto Track = Clip->FindTrack(TrackID);
								if (!Track) continue;

								TrackUniformNameList.emplace(Track->GetParam_String("UniformName"));
							}
						}
					}

					ordered_json materialJSON;

					//
					materialJSON["meshindex"] = MeshIndex;
					materialJSON["primitiveindex"] = PrimitiveIndex;

					// materialframe
					const auto& MaterialFrame = Material->GetMaterialFrame();
					if (!MaterialFrame) return false;
					materialJSON["materialframe"] = MaterialFrame->GetMaterialFrameName();

					// cull
					{
						std::string cull = "";
						graphics::ECullMode CullMode = Material->GetCullMode();
						switch (CullMode)
						{
						case graphics::ECullMode::CULL_NONE:
							cull = "none";
							break;
						case graphics::ECullMode::CULL_BACK:
							cull = "back";
							break;
						case graphics::ECullMode::CULL_FRONT:
							cull = "front";
							break;
						default:
							cull = "back";
							break;
						}

						materialJSON["cull"] = cull;
					}

					// uniformvalues
					{
						const auto& ShaderBufferList = Material->GetShaderBufferList();

						for (auto& UniformBuffer : ShaderBufferList)
						{
							const auto& BufferData = UniformBuffer->GetBuffer();

							const auto& Descriptor = UniformBuffer->GetDescriptor();

							for (const auto& UniformDataMap : Descriptor.GetDataList())
							{
								const auto& UniformData = UniformDataMap.second;

								if (UniformData.ValueInput.Hide) continue;

								// name
								const auto& UniformName = UniformData.UniformName;

								// タイムラインで管理されているなら書き込みをスキップ
								if (TrackUniformNameList.find(UniformName) != TrackUniformNameList.end()) continue;

								// type
								std::string type = std::string();
								graphics::EUniformValueType ValueType = UniformData.ValueType;
								switch (ValueType)
								{
								case graphics::EUniformValueType::NONE:
									break;
								case graphics::EUniformValueType::VALUE_TYPE_MAT4:
									type = "mat4";
									break;
								case graphics::EUniformValueType::VALUE_TYPE_MAT3:
									type = "mat3";
									break;
								case graphics::EUniformValueType::VALUE_TYPE_MAT2:
									type = "mat2";
									break;
								case graphics::EUniformValueType::VALUE_TYPE_VEC4:
									type = "vec4";
									break;
								case graphics::EUniformValueType::VALUE_TYPE_VEC3:
									type = "vec3";
									break;
								case graphics::EUniformValueType::VALUE_TYPE_VEC2:
									type = "vec2";
									break;
								case graphics::EUniformValueType::VALUE_TYPE_FLOAT:
									type = "float";
									break;
								case graphics::EUniformValueType::VALUE_TYPE_INT:
									type = "int";
									break;
								case graphics::EUniformValueType::VALUE_TYPE_FLOAT_ARRAY:
									break;
								case graphics::EUniformValueType::VALUE_TYPE_MAT4_ARRAY:
									break;
								default:
									break;
								}

								if (type.empty()) continue;

								// 初期値
								std::vector<float> InitValue;
								bool ExistInitValue = MaterialFrame->GetDefaultValue(UniformBuffer->GetBufferName(), UniformName, InitValue);


								// Value
								const auto& BufferData = UniformBuffer->GetBuffer();

								ordered_json valueJSON;

								bool ValueUpdated = false; // 値が更新されたかどうか

								if (type == "int")
								{
									std::vector<int> Value;
									Value.resize(UniformData.ByteSize / sizeof(int));
									std::memcpy(&Value[0], &BufferData[UniformData.ByteOffset], UniformData.ByteSize);

									for (int vIndex = 0; vIndex < static_cast<int>(Value.size()); vIndex++)
									{
										auto v = Value[vIndex];

										valueJSON.push_back(v);

										if (ExistInitValue)
										{
											// 初期値との差が0.01よりも大きいパラメーターが1つでもあれば更新された判定にする
											if (fabs(v - InitValue[vIndex]) >= 1)
											{
												ValueUpdated = true;
											}
										}
										else
										{
											// 初期値が存在しないので値は更新されたことにする
											ValueUpdated = true;
										}
									}
								}
								else
								{
									std::vector<float> Value;
									Value.resize(UniformData.ByteSize / sizeof(float));
									std::memcpy(&Value[0], &BufferData[UniformData.ByteOffset], UniformData.ByteSize);

									for (int vIndex = 0; vIndex < static_cast<int>(Value.size()); vIndex++)
									{
										auto v = Value[vIndex];

										valueJSON.push_back(v);

										if (ExistInitValue)
										{
											// 初期値との差が0.01よりも大きいパラメーターが1つでもあれば更新された判定にする
											if (fabs(v - InitValue[vIndex]) >= 0.01)
											{
												ValueUpdated = true;
											}
										}
										else
										{
											// 初期値が存在しないので値は更新されたことにする
											ValueUpdated = true;
										}
									}
								}

								// 値が更新されていなければスキップ
								if (!ValueUpdated) continue;

								//
								materialJSON["uniformvalues"].push_back({
									{ "name", UniformName },
									{ "type", type },
									{ "value", valueJSON}
									});
							}
						}

						// textures
					}

					// textures
					{
						const auto& TextureBindingLayoutList = Material->GetTextureBindingLayoutList();

						for (const auto& TextureBindingLayout : TextureBindingLayoutList)
						{
							int TextureIndex = TextureBindingLayout.TextureIndex;
							std::string RefTextureName = std::string();

							if (TextureBindingLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_2D)
							{
								if (TextureIndex < 0 || TextureIndex >= static_cast<int>(Texture2DList.size())) continue;

								const auto& Texture = Texture2DList[TextureIndex];
								const auto& it = std::find_if(TextureInfoList.begin(), TextureInfoList.end(), [&](const auto& val) { return (Texture == val.second); });

								if (it != TextureInfoList.end())
								{
									RefTextureName = it->first;

									TextureIndex = -1;
								}
							}

							materialJSON["textures"].push_back({
								{ "texturebuffername", TextureBindingLayout.TextureName },
								{ "textureindex", TextureIndex },
								{ "texturename", RefTextureName }
								});
						}
					}

					// Timeline Track ID
					for (const auto& TrackID : Material->GetRefTrackIDList())
					{
						materialJSON["trackids"].push_back(TrackID);
					}

					ObjectJSON["materials"].push_back(materialJSON);
				}
			}
		}

		return true;
	}

	bool CSceneWriter::WriteTextureSet(ordered_json& ObjectJSON, object::C3DObject* pObject, const std::map<std::string, std::shared_ptr<graphics::CTexture>>& TextureInfoList)
	{
		const auto& TextureSet = pObject->GetTextureSet();

		for (const auto& Texture2D : TextureSet->Get2DTextureList())
		{
			const auto& it = std::find_if(TextureInfoList.begin(), TextureInfoList.end(), [&](const auto& val) { return (val.second == Texture2D); });
			if (it == TextureInfoList.end()) continue;

			std::string filter_str = std::string();
			graphics::ETextureFilterMode FilterMode = Texture2D->GetSamplerParam().FilterMode;
			switch (FilterMode)
			{
			case graphics::ETextureFilterMode::NEAREST:
				filter_str = "nearest";
				break;
			case graphics::ETextureFilterMode::LINEAR:
				filter_str = "linear";
				break;
			default:
				break;
			}

			std::string wrap_str = std::string();
			graphics::ETextureWrapMode WrapMode = Texture2D->GetSamplerParam().WrapMode;
			switch (WrapMode)
			{
			case graphics::ETextureWrapMode::REPEAT:
				wrap_str = "repeat";
				break;
			case graphics::ETextureWrapMode::MIRRORED_REPEAT:
				wrap_str = "mirroredrepeat";
				break;
			case graphics::ETextureWrapMode::CLAMP_TO_EDGE:
				wrap_str = "clamptoedge";
				break;
			case graphics::ETextureWrapMode::CLAMP_TO_BORDER:
				wrap_str = "clamptoborder";
				break;
			default:
				break;
			}

			ObjectJSON["textureset"].push_back({
				{ "name", it->first }, 
				{ "filename", Texture2D->GetFileName() },
				{ "mipmap", Texture2D->IsUseMipMap() },
				{ "filter", filter_str },
				{ "wrap", wrap_str }
			});
		}

		return true;
	}

	bool CSceneWriter::WriteAnimation(ordered_json& ObjectJSON, object::C3DObject* pObject, const SAnimationInfo& AnimationInfo)
	{
		json animationJSON;

		const auto& AnimationController = pObject->GetAnimationController();
		const auto& BlendShapeController = pObject->GetBlendShapeController();

		const auto& Clips = AnimationInfo.Clips;
		const auto& HumanoidclipInfoList = AnimationInfo.Humanoidclips;
		const auto& BlendshapeInfoList = AnimationInfo.Blendshapes;

		// clips
		for (const auto& AnimationClip : AnimationController->GetAnimationClipMap())
		{
			const auto& it = Clips.find(AnimationClip.first);
			if (it == Clips.end()) continue;

			animationJSON["clips"].push_back({
				{ "key", AnimationClip.first },
				{ "motionname", it->second.MotionName },
				{ "index",  it->second.Index },
				{ "loop", AnimationClip.second.Clip->IsLoop() }
				});
		}

		if (AnimationController)
		{
			const auto& Skeleton = AnimationController->GetSkeleton();

			if (Skeleton)
			{
				// Rig
				switch (Skeleton->GetRig())
				{
				case animation::ERigType::Humanoid:
					animationJSON["rig"] = "humanoid";
					break;
				default:
					break;
				}

				// HumanoidBoneList
				WriteHumanoidBoneList(animationJSON, Skeleton);
			}

			for (const auto& AnimationClip : AnimationController->GetAnimationClipMap())
			{
				const auto& it = HumanoidclipInfoList.find(AnimationClip.first);
				if (it == HumanoidclipInfoList.end()) continue;

				animationJSON["humanoidclips"].push_back({
					{ "key", AnimationClip.first },
					{ "motionname", it->second.MotionName },
					{ "index",  it->second.Index },
					{ "loop", AnimationClip.second.Clip->IsLoop() },
					{ "ik", AnimationClip.second.Clip->IsUseIK() }
				});
			}

			animationJSON["playmotionindex"] = AnimationController->GetCurrentMotionIndex();
			animationJSON["playmotion"] = AnimationController->GetCurrentMotionName();
		}

		if (BlendShapeController)
		{
			for (const auto& BlendShapeClip : BlendShapeController->GetBlendShapeClipMap())
			{
				const auto& it = BlendshapeInfoList.find(BlendShapeClip.first);
				if (it == BlendshapeInfoList.end()) continue;

				animationJSON["blendshapes"].push_back({
					{ "key", BlendShapeClip.first },
					{ "motionname", it->second.MotionName },
					{ "index", it->second.Index },
					{ "loop", BlendShapeClip.second->IsLoop()}
				});
			}

			for (const auto& PlayingBlendShape : BlendShapeController->GetPlayingBlendShapeSet())
			{
				animationJSON["playblendshapes"].push_back(PlayingBlendShape);
			}
		}

		ObjectJSON["animation"] = animationJSON;

		return true;
	}

	bool CSceneWriter::WriteJoints(ordered_json& ObjectJSON, object::C3DObject* pObject)
	{
		// 物理オブジェクトとノードインデックスのペアを作成
		std::map<std::shared_ptr<physics::IPhysicsObject>, int> PhysObjIndexList;
		for (const auto& Node : pObject->GetNodeList())
		{
			// シーン側では単一の物理オブジェクトしか考慮していない
			if (Node->GetPhysicsObjectList().empty()) continue;

			if (Node->GetSelfNodeIndex() == -1) continue;

			PhysObjIndexList.emplace(Node->GetPhysicsObjectList()[0], Node->GetSelfNodeIndex());
		}

		// Jointを登録
		for (const auto& Node : pObject->GetNodeList())
		{
			// シーン側では単一の物理オブジェクトしか考慮していない
			if (Node->GetPhysicsObjectList().empty()) continue;

			if (Node->GetSelfNodeIndex() == -1) continue;

			const auto& FreePhysicsObject = Node->GetPhysicsObjectList()[0];
			int FreeNodeIndex = Node->GetSelfNodeIndex();

			for (const auto& Constraint : FreePhysicsObject->GetConstraintList())
			{
				const auto it = PhysObjIndexList.find(Constraint->FixedObject);
				if (it == PhysObjIndexList.end()) continue;

				int FixedNodeIndex = it->second;

				physics::EJointType JointType = Constraint->JointType;
				std::string JointTypeStr = std::string();
				
				switch (JointType)
				{
				case physics::EJointType::NONE:
					break;
				case physics::EJointType::SPRING_6DOF:
					JointTypeStr = "spring_6dof";
					break;
				case physics::EJointType::Generic_6DOF:
					JointTypeStr = "generic_6dof";
					break;
				case physics::EJointType::P2P:
					JointTypeStr = "p2p";
					break;
				case physics::EJointType::ConeTwist:
					JointTypeStr = "cone_twist";
					break;
				case physics::EJointType::Slider:
					JointTypeStr = "slider";
					break;
				default:
					break;
				}

				if (JointTypeStr.empty()) continue;

				// 書き出し
				ordered_json jointJSON;

				jointJSON["fixednode"] = FixedNodeIndex;
				jointJSON["freenode"] = FreeNodeIndex;
				jointJSON["jointtype"] = JointTypeStr;
				jointJSON["jointpos"] = { Constraint->JParam.JointPos.x, Constraint->JParam.JointPos.y, Constraint->JParam.JointPos.z };
				jointJSON["jointrotate"] = { Constraint->JParam.JointRotate.x, Constraint->JParam.JointRotate.y, Constraint->JParam.JointRotate.z };
				jointJSON["lowertranslimit"] = { Constraint->JParam.LowerTransLimit.x, Constraint->JParam.LowerTransLimit.y, Constraint->JParam.LowerTransLimit.z };
				jointJSON["uppertranslimit"] = { Constraint->JParam.UpperTransLimit.x, Constraint->JParam.UpperTransLimit.y, Constraint->JParam.UpperTransLimit.z };
				jointJSON["lowerrotatelimit"] = { Constraint->JParam.LowerRotateLimit.x, Constraint->JParam.LowerRotateLimit.y, Constraint->JParam.LowerRotateLimit.z };
				jointJSON["upperrotatelimit"] = { Constraint->JParam.UpperRotateLimit.x, Constraint->JParam.UpperRotateLimit.y, Constraint->JParam.UpperRotateLimit.z };
				jointJSON["transspring"] = { Constraint->JParam.TransSpring.x, Constraint->JParam.TransSpring.y, Constraint->JParam.TransSpring.z };
				jointJSON["rotatespring"] = { Constraint->JParam.RotateSpring.x, Constraint->JParam.RotateSpring.y, Constraint->JParam.RotateSpring.z };

				ObjectJSON["joints"].push_back(jointJSON);
			}
		}

		return true;
	}

	bool CSceneWriter::WriteHumanoidBoneList(json& AnimationJSON, const std::shared_ptr<animation::CSkeleton>& Skeleton)
	{
		const auto& BoneTable = Skeleton->GetHumanoidBoneTable();

		for (int n = 0; n < static_cast<int>(animation::EHumanoidBones::Max); n++)
		{
			animation::EHumanoidBones BoneName = static_cast<animation::EHumanoidBones>(n);

			const auto& it = BoneTable.find(BoneName);
			if (it == BoneTable.end()) continue;

			std::string BoneNameStr = animation::CSkeleton::CastHumanoidBonesToString(BoneName);
			if (BoneNameStr.empty()) continue;

			json boneJson{};
			boneJson["bonename"] = BoneNameStr;
			boneJson["nodename"] = it->second->GetBoneNode()->GetName();

			AnimationJSON["humanbonelist"].emplace_back(boneJson);
		}

		return true;
	}
}
#endif // USE_BINARY_WRITE