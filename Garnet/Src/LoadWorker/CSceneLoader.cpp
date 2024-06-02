#include "CSceneLoader.h"
#include "CLoadWorker.h"
#include "C3DObjectLoader.h"
#include "CMaterialFrameLoader.h"
#include "CTextureLoader.h"
#include "CAnimationLoader.h"
#include "CAudioLoader.h"
#include "../Scene/CSceneController.h"
#include "../Object/C3DObject.h"
#include "../../Message/Console.h"

namespace resource
{
	CSceneLoader::CSceneLoader(const std::string& FileName, const std::shared_ptr<scene::CSceneController>& Target):
		CResource(FileName),
		m_Target(Target)
	{
	}

	CSceneLoader::~CSceneLoader()
	{
	}

	bool CSceneLoader::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		if (!m_File->IsLoaded()) return true;

		// シーン読み込み
		if (!AnalyseScene(pGraphicsAPI, pLoadWorker)) return false;

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		return true;
	}

	bool CSceneLoader::AnalyseScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		std::string RawData = std::string();
		RawData.resize(m_File->GetData().size());
		std::memcpy(&RawData[0], &m_File->GetData()[0], m_File->GetData().size());

		json SceneJSON = json::parse(RawData.c_str());

		// materialframes
		{
			const auto materialframes = SceneJSON.find("materialframes");
			if (materialframes != SceneJSON.end() && materialframes->is_array())
			{
				if (!AnalyseMaterialFrames(materialframes, pLoadWorker)) return false;
			}
		}

		// scenetextureset
		{
			const auto scenetexturesetJSON = SceneJSON.find("scenetextureset");
			if (scenetexturesetJSON != SceneJSON.end() && scenetexturesetJSON->is_object())
			{
				std::shared_ptr<graphics::CTextureSet> SceneTextureSet = std::make_shared<graphics::CTextureSet>();

				if (!AnalyseSceneTextureSet(scenetexturesetJSON, pGraphicsAPI, pLoadWorker, SceneTextureSet)) return false;

				m_Target->SetSceneTextureSet(SceneTextureSet);
			}
		}

		// animations
		{
			const auto animations = SceneJSON.find("animations");
			if (animations != SceneJSON.end() && animations->is_array())
			{
				if (!AnalyseSceneAnimations(animations, pGraphicsAPI, pLoadWorker)) return false;
			}
		}


		// sound
		{
			const auto sound = SceneJSON.find("sound");
			if (sound != SceneJSON.end() && sound->is_object())
			{
				// bgm
				{
					const auto bgm = sound->find("bgm");
					if (bgm != sound->end() && bgm->is_object())
					{
						std::string filename = "";
						GetString("filename", filename, bgm);

						bool autoplay = false;
						GetBoolean("autoplay", autoplay, bgm);
						
						bool loop = false;
						GetBoolean("loop", loop, bgm);

						std::shared_ptr<audio::CAudioClip> AudioClip = std::make_shared<audio::CAudioClip>();

						pLoadWorker->AddLoadResource(std::make_shared<resource::CAudioLoader>(filename, AudioClip));

						m_Target->AddBGM(AudioClip, autoplay, loop);
					}
				}
			}
		}

		// objects
		{
			const auto objects = SceneJSON.find("objects");
			if (objects != SceneJSON.end() && objects->is_array())
			{
				if (!AnalyseObjects(objects, pGraphicsAPI, pLoadWorker)) return false;
			}
		}

		return true;
	}

	bool CSceneLoader::AnalyseMaterialFrames(const json::iterator& materialframes, resource::CLoadWorker* pLoadWorker)
	{
		for (json::iterator mfJSON = materialframes->begin(); mfJSON != materialframes->end(); mfJSON++)
		{
			if (!mfJSON->is_object()) continue;

			std::string MFName = "";
			GetString("name", MFName, mfJSON);

			std::string filename = "";
			GetString("filename", filename, mfJSON);

			if (MFName.empty() || filename.empty()) continue;

			std::shared_ptr<graphics::CMaterialFrame> MaterialFrame = std::make_shared<graphics::CMaterialFrame>();

			pLoadWorker->AddLoadResource(std::make_shared<resource::CMaterialFrameLoader>(filename, MaterialFrame));
			m_Target->AddMaterialFrame(MFName, MaterialFrame);
		}

		return true;
	}

	bool CSceneLoader::AnalyseSceneTextureSet(const json::iterator& scenetexturesetJSON, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, std::shared_ptr<graphics::CTextureSet>& SceneTextureSet)
	{
		// cubemaps
		const auto cubemaps = scenetexturesetJSON->find("cubemaps");
		if (cubemaps != scenetexturesetJSON->end() && cubemaps->is_array())
		{
			for (json::iterator cubemapJSON = cubemaps->begin(); cubemapJSON != cubemaps->end(); cubemapJSON++)
			{
				std::string back = "";
				GetString("back", back, cubemapJSON);

				std::string bottom = "";
				GetString("bottom", bottom, cubemapJSON);

				std::string front = "";
				GetString("front", front, cubemapJSON);

				std::string left = "";
				GetString("left", left, cubemapJSON);

				std::string right = "";
				GetString("right", right, cubemapJSON);

				std::string top = "";
				GetString("top", top, cubemapJSON);

				if (back.empty() || bottom.empty() || front.empty() || left.empty() || right.empty() || top.empty()) continue;

				std::vector<std::string> FileNameList = { back, bottom, front, left, right, top };

				auto Texture = pGraphicsAPI->CreateTexture();
				pLoadWorker->AddLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, FileNameList, Texture));
				SceneTextureSet->AddCubeMap(Texture);
			}
		}

		// ibl
		const auto ibl = scenetexturesetJSON->find("ibl");
		if (ibl != scenetexturesetJSON->end() && ibl->is_object())
		{
			std::string diffuse = "";
			GetString("diffuse", diffuse, ibl);

			std::string specular = "";
			GetString("specular", specular, ibl);

			std::string ggx = "";
			GetString("ggx", ggx, ibl);

			if (!diffuse.empty() && !specular.empty() && !ggx.empty())
			{
				auto diffuseTexture = pGraphicsAPI->CreateTexture();
				pLoadWorker->AddLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, diffuse, diffuseTexture));

				auto specularTexture = pGraphicsAPI->CreateTexture();
				pLoadWorker->AddLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, specular, specularTexture));

				auto ggxTexture = pGraphicsAPI->CreateTexture();
				pLoadWorker->AddLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, ggx, ggxTexture));

				SceneTextureSet->AddIBLTexture(diffuseTexture, specularTexture, ggxTexture);
			}
		}

		return true;
	}

	bool CSceneLoader::AnalyseSceneAnimations(const json::iterator& animations, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		for (json::iterator animationJSON = animations->begin(); animationJSON != animations->end(); animationJSON++)
		{
			if (!animationJSON->is_object()) continue;

			std::string name = "";
			GetString("name", name, animationJSON);

			std::string filename = "";
			GetString("filename", filename, animationJSON);

			std::shared_ptr<animation::CAnimationClipSet> AnimationClipSet = std::make_shared<animation::CAnimationClipSet>();

			pLoadWorker->AddLoadResource(std::make_shared<resource::CAnimationLoader>(filename, AnimationClipSet));

			m_Target->AddAnimationClipSet(name, AnimationClipSet);
		}

		return true;
	}

	bool CSceneLoader::AnalyseObjects(const json::iterator& objects, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		for (json::iterator objectJSON = objects->begin(); objectJSON != objects->end(); objectJSON++)
		{
			if (!objectJSON->is_object()) continue;

			std::shared_ptr<object::C3DObject> Object = std::make_shared<object::C3DObject>("", "");

			// ObjectName
			{
				std::string objname = "";
				GetString("name", objname, objectJSON);

				Object->SetObjectName(objname);
			}

			{
				std::string filename = "";
				GetString("filename", filename, objectJSON);

				std::string commonmaterialframe = "";
				GetString("commonmaterialframe", commonmaterialframe, objectJSON);

				if (!filename.empty())
				{
					const auto& MaterialFrameMap = m_Target->GetMaterialFrameMap();
					const auto& MaterialFrame = MaterialFrameMap.find(commonmaterialframe);
					if (MaterialFrame == MaterialFrameMap.end())
					{
						Console::Log("[SceneLoader Error] commonmaterialframe not found\n");

						return false;
					}

					pLoadWorker->AddLoadResource(std::make_shared<resource::C3DObjectLoader>(filename, Object, MaterialFrame->second));
				}
			}

			// Transform
			{
				auto Transform = AnalyseTransform(objectJSON);

				Object->SetPos(Transform->GetPos());
				Object->SetRot(Transform->GetRot());
				Object->SetScale(Transform->GetScale());
			}

			// rootnodes
			{
				// ToDo: この2次元配列のIndexListは害悪なのでリファクタリングしたい
				std::vector<std::vector<int>> RootNodeIndexList;

				std::vector<int> rootnodes;

				GetArrayInt32("rootnodes", rootnodes, objectJSON);

				for (int root : rootnodes)
				{
					RootNodeIndexList.push_back(std::vector<int>({ root }));
				}

				Object->SetRootNodeIndexList(RootNodeIndexList);
			}

			// nodes
			const auto nodes = objectJSON->find("nodes");
			if (nodes != objectJSON->end() && nodes->is_array())
			{
				for (json::iterator nodeJSON = nodes->begin(); nodeJSON != nodes->end(); nodeJSON++)
				{
					if (!nodeJSON->is_object()) continue;

					std::shared_ptr<object::CNode> Node = AnalyseNode(nodeJSON, Object);
					Object->AddNode(Node);
				}
			}

			// meshs
			std::map<int, int> MatRefCountMap;

			const auto meshs = objectJSON->find("meshs");
			if (meshs != objectJSON->end() && meshs->is_array())
			{
				for (json::iterator meshJSON = meshs->begin(); meshJSON != meshs->end(); meshJSON++)
				{
					if (!meshJSON->is_object()) continue;

					std::shared_ptr<graphics::CMesh> Mesh = AnalyseMesh(pGraphicsAPI, meshJSON, MatRefCountMap);
					Object->AddMesh(Mesh);
				}
			}

			// materials
			const auto materials = objectJSON->find("materials");
			if (materials != objectJSON->end() && materials->is_array())
			{
				std::vector<scene::SMaterialInfo> MaterialInfoList;

				for (json::iterator materialJSON = materials->begin(); materialJSON != materials->end(); materialJSON++)
				{
					if (!materialJSON->is_object()) continue;

					int MaterialIndex = static_cast<int>(MaterialInfoList.size());

					scene::SMaterialInfo MaterialInfo = AnalyseMaterialInfo(materialJSON, MaterialIndex, MatRefCountMap);
					MaterialInfoList.push_back(MaterialInfo);
				}

				// SceneControllerに登録
				m_Target->AddMaterialInfo(Object, MaterialInfoList);
			}

			// textureset
			const auto textureset = objectJSON->find("textureset");
			if (textureset != objectJSON->end() && textureset->is_array())
			{
				std::map<std::string, std::shared_ptr<graphics::CTexture>> TextureInfoList;

				for (json::iterator textureJSON = textureset->begin(); textureJSON != textureset->end(); textureJSON++)
				{
					if (!textureJSON->is_object()) continue;

					std::string TextureName = "";
					GetString("name", TextureName, textureJSON);

					std::string TextureFileName = "";
					GetString("filename", TextureFileName, textureJSON);

					auto Texture = pGraphicsAPI->CreateTexture();
					pLoadWorker->AddLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, TextureFileName, Texture));

					TextureInfoList.emplace(TextureName, Texture);
				}

				m_Target->AddTextureInfo(Object, TextureInfoList);
			}

			// animation
			const auto animationJSON = objectJSON->find("animation");
			if (animationJSON != objectJSON->end() && animationJSON->is_object())
			{
				scene::SAnimationInfo AnimationInfo = AnalyseAnimationInfo(animationJSON);

				m_Target->AddAnimationInfo(Object, AnimationInfo);
			}

			// SceneTextureSet
			const auto& SceneTextureSet = m_Target->GetSceneTextureSet();
			if (SceneTextureSet)
			{
				for (const auto& CubeMap : SceneTextureSet->GetCubeMapList())
				{
					Object->GetTextureSet()->AddCubeMap(CubeMap);
				}

				const auto& Diffuse_Tex = SceneTextureSet->GetDiffuse_Tex();
				const auto& Specular_Tex = SceneTextureSet->GetSpecular_Tex();
				const auto& GGXLUT_Tex = SceneTextureSet->GetGGXLUT_Tex();
				if (Diffuse_Tex && Specular_Tex && GGXLUT_Tex)
				{
					Object->GetTextureSet()->AddIBLTexture(Diffuse_Tex, Specular_Tex, GGXLUT_Tex);
				}
			}

			// Objectを追加
			m_Target->AddObject(Object);
		}

		return true;
	}

	std::shared_ptr<object::CNode> CSceneLoader::AnalyseNode(const json::iterator& nodeJSON, const std::shared_ptr<object::C3DObject>& Object)
	{
		std::string nodename = "";
		GetString("name", nodename, nodeJSON);

		auto Transform = AnalyseTransform(nodeJSON);

		int meshindex = -1;
		GetInt("meshindex", meshindex, nodeJSON);

		std::vector<int> children;
		GetArrayInt32("children", children, nodeJSON);

		// ノードを作成
		int SelfNodeIndex = static_cast<int>(Object->GetNodeList().size());

		std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(meshindex, SelfNodeIndex);

		Node->SetName(nodename);
		Node->SetLocalTransform(Transform);
		Node->SetChildrenNodeIndexList(children);

		return Node;
	}

	std::shared_ptr<graphics::CMesh> CSceneLoader::AnalyseMesh(api::IGraphicsAPI* pGraphicsAPI, const json::iterator& meshJSON, std::map<int, int>& MatRefCountMap)
	{
		std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

		const auto primitives = meshJSON->find("primitives");
		if (primitives != meshJSON->end() && primitives->is_array())
		{
			for (json::iterator primitiveJSON = primitives->begin(); primitiveJSON != primitives->end(); primitiveJSON++)
			{
				if (!primitiveJSON->is_object()) continue;

				int materialindex = -1;
				GetInt("materialindex", materialindex, primitiveJSON);

				// マテリアル参照数の追加
				{
					auto it = MatRefCountMap.find(materialindex);

					if (it == MatRefCountMap.end()) MatRefCountMap.emplace(materialindex, 0);

					MatRefCountMap[materialindex]++;
				}

				std::string type = "";
				GetString("type", type, primitiveJSON);

				graphics::EPresetPrimitiveType PrimitiveType = graphics::EPresetPrimitiveType::None;

				std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> createInfo;

				if (type == "cube")
				{
					createInfo = graphics::CPresetPrimitive::CreateBox(pGraphicsAPI);
					PrimitiveType = graphics::EPresetPrimitiveType::CUBE;
				}
				else if (type == "board")
				{
					createInfo = graphics::CPresetPrimitive::CreateBoard(pGraphicsAPI);
					PrimitiveType = graphics::EPresetPrimitiveType::BOARD;
				}
				else if (type == "sphere")
				{
					createInfo = graphics::CPresetPrimitive::CreateSphere(pGraphicsAPI);
					PrimitiveType = graphics::EPresetPrimitiveType::SPHERE;
				}
				else if (type == "point")
				{
					createInfo = graphics::CPresetPrimitive::CreatePoint(pGraphicsAPI);
					PrimitiveType = graphics::EPresetPrimitiveType::POINT;
				}
				else
				{
					createInfo = graphics::CPresetPrimitive::CreateBox(pGraphicsAPI);
					PrimitiveType = graphics::EPresetPrimitiveType::CUBE;
				}

				//
				Mesh->CreatePresetSimpleMesh(createInfo.first, createInfo.second, materialindex, PrimitiveType);
			}
		}

		return Mesh;
	}

	scene::SMaterialInfo CSceneLoader::AnalyseMaterialInfo(const json::iterator& materialJSON, int MaterialIndex, const std::map<int, int>& MatRefCountMap)
	{
		scene::SMaterialInfo MaterialInfo{};

		// マテリアルフレーム名
		std::string materialframe = "";
		GetString("materialframe", materialframe, materialJSON);

		MaterialInfo.MaterialFrameName = materialframe;

		// マテリアル参照数
		{
			auto it = MatRefCountMap.find(MaterialIndex);

			if (it != MatRefCountMap.end())
			{
				MaterialInfo.RefCount = it->second;
			}
		}

		// Uniformリスト
		const auto uniformvalues = materialJSON->find("uniformvalues");
		if (uniformvalues != materialJSON->end() && uniformvalues->is_array())
		{
			for (json::iterator uniformJSON = uniformvalues->begin(); uniformJSON != uniformvalues->end(); uniformJSON++)
			{
				scene::SUniformInfo UniformInfo{};

				std::string UniformName = "";
				GetString("name", UniformName, uniformJSON);
				UniformInfo.UniformName = UniformName;

				std::string UniformType = "";
				GetString("type", UniformType, uniformJSON);

				// ByteSize
				int ByteSize = 0;
				{
					if (UniformType == "mat4")
					{
						ByteSize = sizeof(glm::mat4);
					}
					else if (UniformType == "mat3")
					{
						ByteSize = sizeof(glm::mat3);
					}
					else if (UniformType == "mat2")
					{
						ByteSize = sizeof(glm::mat2);
					}
					else if (UniformType == "vec4")
					{
						ByteSize = sizeof(glm::vec4);
					}
					else if (UniformType == "vec3")
					{
						ByteSize = sizeof(glm::vec3);
					}
					else if (UniformType == "vec2")
					{
						ByteSize = sizeof(glm::vec2);
					}
					else if (UniformType == "float")
					{
						ByteSize = sizeof(float);
					}
					else if (UniformType == "int")
					{
						ByteSize = sizeof(int);
					}

					UniformInfo.ByteSize = ByteSize;
				}

				//
				std::vector<float> value;
				GetArrayFloat32("value", value, uniformJSON);

				std::vector<unsigned char> UniformData;
				UniformData.resize(ByteSize);

				std::memcpy(&UniformData[0], &value[0], ByteSize);

				UniformInfo.UniformData = UniformData;

				//
				MaterialInfo.UniformInfoList.push_back(UniformInfo);
			}
		}

		// CullMode
		{
			std::string cull = "";
			GetString("cull", cull, materialJSON);

			if (cull == "none")
			{
				MaterialInfo.CullMode = graphics::ECullMode::CULL_NONE;
			}
			else if (cull == "back")
			{
				MaterialInfo.CullMode = graphics::ECullMode::CULL_BACK;
			}
			else if (cull == "front")
			{
				MaterialInfo.CullMode = graphics::ECullMode::CULL_FRONT;
			}
			else
			{
				MaterialInfo.CullMode = graphics::ECullMode::CULL_BACK;
			}
		}

		// textures
		const auto textures = materialJSON->find("textures");
		if (textures != materialJSON->end() && textures->is_array())
		{
			for (json::iterator textureJSON = textures->begin(); textureJSON != textures->end(); textureJSON++)
			{
				if (!textureJSON->is_object()) continue;

				std::string texturebuffername = "";
				GetString("texturebuffername", texturebuffername, textureJSON);

				std::string texturename = "";
				GetString("texturename", texturename, textureJSON);

				int textureindex = -1;
				GetInt("textureindex", textureindex, textureJSON);

				MaterialInfo.Textures.push_back(std::make_tuple(texturebuffername, texturename, textureindex));
			}
		}

		return MaterialInfo;
	}

	scene::SAnimationInfo CSceneLoader::AnalyseAnimationInfo(const json::iterator& animationJSON)
	{
		scene::SAnimationInfo AnimationInfo{};

		// clips
		const auto clips = animationJSON->find("clips");
		if (clips != animationJSON->end() && clips->is_array())
		{
			// 未実装
		}

		// humanoidclips
		const auto humanoidclips = animationJSON->find("humanoidclips");
		if (humanoidclips != animationJSON->end() && humanoidclips->is_array())
		{
			for (json::iterator humanoidJSON = humanoidclips->begin(); humanoidJSON != humanoidclips->end(); humanoidJSON++)
			{
				if (!humanoidJSON->is_object()) continue;
				
				scene::SHumanoidclip Clip{};

				std::string key = "";
				GetString("key", key, humanoidJSON);
				Clip.Key = key;

				std::string motionname = "";
				GetString("motionname", motionname, humanoidJSON);
				Clip.MotionName = motionname;

				int index = -1;
				GetInt("index", index, humanoidJSON);
				Clip.Index = index;

				bool loop = false;
				GetBoolean("loop", loop, humanoidJSON);
				Clip.Loop = loop;

				bool ik = false;
				GetBoolean("ik", ik, humanoidJSON);
				Clip.IK = ik;

				AnimationInfo.Humanoidclips.push_back(Clip);
			}
		}

		// blendshapes
		const auto blendshapes = animationJSON->find("blendshapes");
		if (blendshapes != animationJSON->end() && blendshapes->is_array())
		{
			for (json::iterator blendshapeJSON = blendshapes->begin(); blendshapeJSON != blendshapes->end(); blendshapeJSON++)
			{
				if (!blendshapeJSON->is_object()) continue;

				scene::SBlendshape Clip{};

				std::string key = "";
				GetString("key", key, blendshapeJSON);
				Clip.Key = key;

				std::string motionname = "";
				GetString("motionname", motionname, blendshapeJSON);
				Clip.MotionName = motionname;

				int index = -1;
				GetInt("index", index, blendshapeJSON);
				Clip.Index = index;

				bool loop = false;
				GetBoolean("loop", loop, blendshapeJSON);
				Clip.Loop = loop;

				AnimationInfo.Blendshapes.push_back(Clip);
			}
		}

		// playmotion
		std::string playmotion = "";
		GetString("playmotion", playmotion, animationJSON);
		AnimationInfo.PlayMotion = playmotion;

		// playmotionindex
		int playmotionindex = -1;
		GetInt("playmotionindex", playmotionindex, animationJSON);
		AnimationInfo.PlayMotionIndex = playmotionindex;

		// playblendshapes
		const auto playblendshapes = animationJSON->find("playblendshapes");
		if (playblendshapes != animationJSON->end() && playblendshapes->is_array())
		{
			for (json::iterator blendshape = playblendshapes->begin(); blendshape != playblendshapes->end(); blendshape++)
			{
				std::string Key = blendshape.value();
				AnimationInfo.PlayBlendShapes.push_back(Key);
			}
		}

		return AnimationInfo;
	}

	std::shared_ptr<math::CTransform> CSceneLoader::AnalyseTransform(const json::iterator& Object)
	{
		std::shared_ptr<math::CTransform> Transform = std::make_shared<math::CTransform>();

		const auto transformJSON = Object->find("transform");
		if (transformJSON->is_object())
		{
			// pos
			glm::vec3 pos = glm::vec3(0.0f);
			{
				std::vector<float> posArray;
				GetArrayFloat32("pos", posArray, transformJSON);

				if (posArray.size() == 3)
				{
					pos = glm::vec3(posArray[0], posArray[1], posArray[2]);
				}
			}

			// rotate
			glm::quat rotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			{
				std::vector<float> rotateArray;
				GetArrayFloat32("rotate", rotateArray, transformJSON);

				if (rotateArray.size() == 3)
				{
					// rotateArrayはDegreeである
					rotate =
						glm::angleAxis(glm::radians(rotateArray[2]), glm::vec3(0.0f, 0.0f, 1.0f)) *
						glm::angleAxis(glm::radians(rotateArray[1]), glm::vec3(0.0f, 1.0f, 0.0f)) *
						glm::angleAxis(glm::radians(rotateArray[0]), glm::vec3(1.0f, 0.0f, 0.0f));
				}
			}

			// scale
			glm::vec3 scale = glm::vec3(1.0f);
			{
				std::vector<float> scaleArray;
				GetArrayFloat32("scale", scaleArray, transformJSON);

				if (scaleArray.size() == 3)
				{
					scale = glm::vec3(scaleArray[0], scaleArray[1], scaleArray[2]);
				}
			}

			// 登録
			Transform->SetPos(pos);
			Transform->SetRot(rotate);
			Transform->SetScale(scale);
		}

		return Transform;
	}

	void CSceneLoader::GetString(const std::string& Key, std::string& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_string())
		{
			Value = it.value();
		}
	}

	void CSceneLoader::GetBoolean(const std::string& Key, bool& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_boolean())
		{
			Value = it.value();
		}
	}

	void CSceneLoader::GetInt(const std::string& Key, int& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_number_integer())
		{
			Value = it.value();
		}
	}

	void CSceneLoader::GetArrayInt32(const std::string& Key, std::vector<int>& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_array())
		{
			for (json::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				if (it2->is_number())
				{
					int val = it2.value();

					Value.push_back(val);
				}
			}
		}
	}

	void CSceneLoader::GetFloat(const std::string& Key, float& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_number_float())
		{
			Value = it.value();
		}
	}

	void CSceneLoader::GetArrayFloat32(const std::string& Key, std::vector<float>& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_array())
		{
			for (json::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				if (it2->is_number())
				{
					float val = it2.value();

					Value.push_back(val);
				}
			}
		}
	}
}