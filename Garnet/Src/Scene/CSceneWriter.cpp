#include "CSceneWriter.h"
#include "CSceneController.h"
#include "../Object/C3DObject.h"
#include "../Animation/CAnimationClipSet.h"
#include "../Audio/CAudioClip.h"
#include "../LoadWorker/CFile.h"

namespace scene
{
	bool CSceneWriter::Write(CSceneController* pSceneController)
	{
		json SceneJSON;

		// シーンJSONに現在の状態を書き出し
		if (!WriteScene(SceneJSON, pSceneController)) return false;

		// データを取得
		const auto& JSONStr = SceneJSON.dump(4);

		std::vector<unsigned char> Data;
		Data.resize(JSONStr.size());

		std::memcpy(&Data[0], &JSONStr[0], static_cast<int>(JSONStr.size()));

		// ファイル書き出し
		resource::CFile File = resource::CFile("Resources\\Scene\\SceneWriteTest.json");
		File.SetData(Data);
		if (!File.Write()) return false;

		return true;
	}

	bool CSceneWriter::WriteScene(json& SceneJSON, CSceneController* pSceneController)
	{
		if (!WriteMaterialFrames(SceneJSON, pSceneController)) return false;
		if (!WriteSceneTextureSet(SceneJSON, pSceneController)) return false;
		if (!WriteAnimations(SceneJSON, pSceneController)) return false;
		if (!WriteSound(SceneJSON, pSceneController)) return false;
		if (!WriteObjects(SceneJSON, pSceneController)) return false;

		return true;
	}

	bool CSceneWriter::WriteMaterialFrames(json& SceneJSON, CSceneController* pSceneController)
	{
		const auto& MaterialFrameMap = pSceneController->GetMaterialFrameMap();

		for (const auto& MaterialFrame : MaterialFrameMap)
		{
			SceneJSON["materialframes"].push_back({ {"name" , MaterialFrame.first}, { "filename", MaterialFrame.second->GetFileName()} });
		}

		return true;
	}

	bool CSceneWriter::WriteSceneTextureSet(json& SceneJSON, CSceneController* pSceneController)
	{
		const auto& SceneTextureSet = pSceneController->GetSceneTextureSet();

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

	bool CSceneWriter::WriteAnimations(json& SceneJSON, CSceneController* pSceneController)
	{
		const auto& AnimationClipSetMap = pSceneController->GetAnimationClipSetMap();

		for (const auto& AnimationClipSet : AnimationClipSetMap)
		{
			SceneJSON["animations"].push_back({ {"name" , AnimationClipSet.first}, { "filename", AnimationClipSet.second->GetFileName()} });
		}

		return true;
	}

	bool CSceneWriter::WriteSound(json& SceneJSON, CSceneController* pSceneController)
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

	bool CSceneWriter::WriteObjects(json& SceneJSON, CSceneController* pSceneController)
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

			// ToDo: ひとまずFileはいったん後回し
			if (StoredFile3DModel) continue;

			json ObjectJSON;

			ObjectJSON["name"] = Object->GetObjectName();
			ObjectJSON["filename"] = Object->GetFileName();

			// ToDo: commonmaterialframe

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

			ObjectJSON["enable"] = Object->IsEnabled();

			// rootnodes
			if (!StoredFile3DModel)
			{
				for (const auto& RootNodes : Object->GetRootNodeIndexList())
				{
					for (int Root : RootNodes)
					{
						ObjectJSON["rootnodes"].push_back(Root);
					}
				}
			}

			// nodes
			if (!StoredFile3DModel)
			{
				if (!WriteNodes(ObjectJSON, Object.get())) return false;
			}

			// meshs
			if (!StoredFile3DModel)
			{
				if (!WriteMeshs(ObjectJSON, Object.get())) return false;
			}

			// materials
			{
				if (!WriteMaterials(ObjectJSON, Object.get(), TextureInfoList)) return false;
			}

			// textureset
			{
				if (!WriteTextureSet(ObjectJSON, Object.get(), TextureInfoList)) return false;
			}

			// objectsに追加
			SceneJSON["objects"].push_back(ObjectJSON);
		}

		return true;
	}

	bool CSceneWriter::WriteNodes(json& ObjectJSON, object::C3DObject* pObject)
	{
		const auto& NodeList = pObject->GetNodeList();

		for (const auto& Node : NodeList)
		{
			json node;

			node["name"] = Node->GetName();

			// transform
			{
				const auto& transform = Node->GetLocalTransform();
				const auto& rotate = glm::eulerAngles(transform->GetRot());

				node["transform"] = {
					{ "pos", {transform->GetPos().x, transform->GetPos().y, transform->GetPos().z} },
					{ "rotate", {glm::degrees(rotate.x), glm::degrees(rotate.y), glm::degrees(rotate.z)} },
					{ "scale", {transform->GetScale().x, transform->GetScale().y, transform->GetScale().z} }
				};
			}

			node["meshindex"] = Node->GetMeshIndex();

			for (int ChildIndex : Node->GetChildrenNodeIndexList())
			{
				node["children"].push_back(ChildIndex);
			}

			ObjectJSON["nodes"].push_back(node);
		}

		return true;
	}

	bool CSceneWriter::WriteMeshs(json& ObjectJSON, object::C3DObject* pObject)
	{
		const auto& MeshList = pObject->GetMeshList();

		for (const auto& Mesh : MeshList)
		{
			json meshJSON;

			const auto& PrimitiveList = Mesh->GetPrimitiveList();

			for (const auto& Primitive : PrimitiveList)
			{
				json primitiveJSON;

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

				primitiveJSON["materialindex"] = Primitive->GetMaterialIndex();
				primitiveJSON["type"] = type;

				meshJSON["primitives"].push_back(primitiveJSON);
			}

			ObjectJSON["meshs"].push_back(meshJSON);
		}

		return true;
	}

	bool CSceneWriter::WriteMaterials(json& ObjectJSON, object::C3DObject* pObject, const std::map<std::string, std::shared_ptr<graphics::CTexture>>& TextureInfoList)
	{
		const auto& MaterialList = pObject->GetMaterialList();
		const auto& TextureSet = pObject->GetTextureSet();
		const auto& Texture2DList = TextureSet->Get2DTextureList();

		for (const auto& Material : MaterialList)
		{
			json materialJSON;

			// materialframe
			// ToDo: あとでリファクタリングが必要かも
			materialJSON["materialframe"] = Material->GetRefMaterialFrameName();

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
					cull = "none";
					break;
				}

				materialJSON["cull"] = cull;
			}

			// uniformvalues
			{
				const auto& ShaderBufferList = Material->GetShaderBufferList();

				for (auto& UniformBuffer : ShaderBufferList)
				{
					const auto& BufferData = UniformBuffer->GetData();

					const auto& Descriptor = UniformBuffer->GetDescriptor();

					for (const auto& UniformDataMap : Descriptor->GetDataList())
					{
						const auto& UniformData = UniformDataMap.second;

						if (UniformData.ValueInput.Hide) continue;

						// name
						const auto& UniformName = UniformData.UniformName;

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

						// Value
						const auto& BufferData = UniformBuffer->GetData();

						std::vector<float> Value;
						Value.resize(UniformData.ByteSize / sizeof(float));
						std::memcpy(&Value[0], &BufferData[UniformData.ByteOffset], UniformData.ByteSize);

						json valueJSON;

						for (auto v : Value)
						{
							valueJSON.push_back(v);
						}

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
						if (it == TextureInfoList.end()) continue;

						RefTextureName = it->first;

						TextureIndex = -1;
					}

					materialJSON["textures"].push_back({
						{ "texturebuffername", TextureBindingLayout.TextureName },
						{ "textureindex", TextureIndex },
						{ "texturename", RefTextureName }
					});
				}
			}

			ObjectJSON["materials"].push_back(materialJSON);
		}

		return true;
	}

	bool CSceneWriter::WriteTextureSet(json& ObjectJSON, object::C3DObject* pObject, const std::map<std::string, std::shared_ptr<graphics::CTexture>>& TextureInfoList)
	{
		const auto& TextureSet = pObject->GetTextureSet();

		for (const auto& Texture2D : TextureSet->Get2DTextureList())
		{
			const auto& it = std::find_if(TextureInfoList.begin(), TextureInfoList.end(), [&](const auto& val) { return (val.second == Texture2D); });
			if (it == TextureInfoList.end()) continue;

			ObjectJSON["textureset"].push_back({
				{ "name", it->first }, // ToDo: リファクタリングが必要かも
				{ "filename", Texture2D->GetFileName() },
			});
		}

		return true;
	}
}