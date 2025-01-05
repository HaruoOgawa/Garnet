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
		m_Target->SetFileName(FileName);
	}

	CSceneLoader::~CSceneLoader()
	{
	}

	bool CSceneLoader::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::CApp* pApp)
	{
		if (!m_File->IsLoaded()) return true;

		// シーン読み込み
		if (!AnalyseScene(pGraphicsAPI, pPhysicsEngine, pLoadWorker, pApp)) return false;

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		return true;
	}

	bool CSceneLoader::AnalyseScene(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::CApp* pApp)
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

		// valueregistries
		{
			const auto valueregistries = SceneJSON.find("valueregistries");
			if (valueregistries != SceneJSON.end() && valueregistries->is_array())
			{
				if (!AnalyseValueRegistries(valueregistries)) return false;
			}
		}

#ifdef USE_TEXTURE_LOADER
		// scenetextureset
		{
			const auto scenetexturesetJSON = SceneJSON.find("scenetextureset");
			if (scenetexturesetJSON != SceneJSON.end() && scenetexturesetJSON->is_object())
			{
				if (!AnalyseSceneTextureSet(scenetexturesetJSON, pGraphicsAPI, pLoadWorker, m_Target->GetSceneTextureSet())) return false;
			}
		}
#endif // USE_TEXTURE_LOADER

#ifdef USE_ANIMATION
		// animations
		{
			const auto animations = SceneJSON.find("animations");
			if (animations != SceneJSON.end() && animations->is_array())
			{
				if (!AnalyseSceneAnimations(animations, pGraphicsAPI, pLoadWorker)) return false;
			}
		}
#endif // USE_ANIMATION

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

						// ロードワーカーには渡さずにファイル名からネイティブAPIから読む
						if (!AudioClip->CreateFromFile(filename)) return false;
						//pLoadWorker->AddLoadResource(std::make_shared<resource::CAudioLoader>(filename, AudioClip));

						m_Target->AddBGM(AudioClip, autoplay, loop);
					}
				}
			}
		}

		// timeline
		{
			const auto timeline = SceneJSON.find("timeline");
			if (timeline != SceneJSON.end() && timeline->is_object())
			{
				std::string filename = "";
				GetString("filename", filename, timeline);

				m_Target->SetTimelineFileName(filename);
			}
		}

		// physics
		{
			const auto physics = SceneJSON.find("physics");
			if (physics != SceneJSON.end() && physics->is_object())
			{
				bool enabled = true;
				GetBoolean("enabled", enabled, physics);

				if (pPhysicsEngine)
				{
					pPhysicsEngine->SetEnabled(enabled);
				}
			}
		}

		// objects
		{
			const auto objects = SceneJSON.find("objects");
			if (objects != SceneJSON.end() && objects->is_array())
			{
				if (!AnalyseObjects(objects, pGraphicsAPI, pPhysicsEngine, pLoadWorker, pApp)) return false;
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

	bool CSceneLoader::AnalyseValueRegistries(const json::iterator& valueregistries)
	{
		for (json::iterator registryJSON = valueregistries->begin(); registryJSON != valueregistries->end(); registryJSON++)
		{
			std::string registryname = std::string();
			GetString("registryname", registryname, registryJSON);

			std::shared_ptr<scriptable::CValueRegistry> ValueRegistry = std::make_shared<scriptable::CValueRegistry>(registryname);

			// TimelineTrack
			std::vector<std::string> trackids;
			GetArrayString("trackids", trackids, registryJSON);

			ValueRegistry->SetRefTrackIDList(trackids);

			const auto values = registryJSON->find("values");
			if (values != registryJSON->end() && values->is_array())
			{
				for (json::iterator valueJSON = values->begin(); valueJSON != values->end(); valueJSON++)
				{
					std::string name = std::string();
					GetString("name", name, valueJSON);

					graphics::EUniformValueType ValueType = graphics::EUniformValueType::NONE;
					std::string type = std::string();
					GetString("type", type, valueJSON);

					int ByteSize = 0;
					{
						if (type == "mat4")
						{
							ValueType = graphics::EUniformValueType::VALUE_TYPE_MAT4;
							ByteSize = sizeof(glm::mat4);
						}
						else if (type == "mat3")
						{
							ValueType = graphics::EUniformValueType::VALUE_TYPE_MAT3;
							ByteSize = sizeof(glm::mat3);
						}
						else if (type == "mat2")
						{
							ValueType = graphics::EUniformValueType::VALUE_TYPE_MAT2;
							ByteSize = sizeof(glm::mat2);
						}
						else if (type == "vec4")
						{
							ValueType = graphics::EUniformValueType::VALUE_TYPE_VEC4;
							ByteSize = sizeof(glm::vec4);
						}
						else if (type == "vec3")
						{
							ValueType = graphics::EUniformValueType::VALUE_TYPE_VEC3;
							ByteSize = sizeof(glm::vec3);
						}
						else if (type == "vec2")
						{
							ValueType = graphics::EUniformValueType::VALUE_TYPE_VEC2;
							ByteSize = sizeof(glm::vec2);
						}
						else if (type == "float")
						{
							ValueType = graphics::EUniformValueType::VALUE_TYPE_FLOAT;
							ByteSize = sizeof(float);
						}
						else if (type == "int")
						{
							ValueType = graphics::EUniformValueType::VALUE_TYPE_INT;
							ByteSize = sizeof(int);
						}
					}

					std::vector<float> initValue;
					GetArrayFloat32("initValue", initValue, valueJSON);

					std::vector<unsigned char> Buffer;
					Buffer.resize(ByteSize);
					std::memcpy(&Buffer[0], &initValue[0], ByteSize);

					//
					ValueRegistry->SetValue(name, ValueType, &Buffer[0], ByteSize);
				}
			}

			// SceneControllerに登録する
			m_Target->SetValueRegistry(registryname, ValueRegistry);
		}

		return true;
	}

#ifdef USE_TEXTURE_LOADER
	bool CSceneLoader::AnalyseSceneTextureSet(const json::iterator& scenetexturesetJSON, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, 
		std::shared_ptr<graphics::CTextureSet>& SceneTextureSet)
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

				std::vector<std::string> FileNameList = { right, left, top, bottom, front, back };

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
				auto diffuseTexture = pGraphicsAPI->CreateTexture(true, { graphics::ETextureFilterMode::LINEAR, graphics::ETextureWrapMode::REPEAT });
				pLoadWorker->AddLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, diffuse, diffuseTexture));

				auto specularTexture = pGraphicsAPI->CreateTexture(true, { graphics::ETextureFilterMode::LINEAR, graphics::ETextureWrapMode::REPEAT });
				pLoadWorker->AddLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, specular, specularTexture));

				auto ggxTexture = pGraphicsAPI->CreateTexture(false, { graphics::ETextureFilterMode::LINEAR, graphics::ETextureWrapMode::REPEAT });
				pLoadWorker->AddLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, ggx, ggxTexture));

				SceneTextureSet->AddIBLTexture(diffuseTexture, specularTexture, ggxTexture);
			}
		}

		return true;
	}
#endif // USE_TEXTURE_LOADER

#ifdef USE_ANIMATION
	bool CSceneLoader::AnalyseSceneAnimations(const json::iterator& animations, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		for (json::iterator animationJSON = animations->begin(); animationJSON != animations->end(); animationJSON++)
		{
			if (!animationJSON->is_object()) continue;

			// humanoid
			std::string rig = std::string();
			GetString("rig", rig, animationJSON);

			animation::ERigType RigType = animation::ERigType::None;

			if (rig == "humanoid")
			{
				RigType = animation::ERigType::Humanoid;
			}

			// humanbonelist
			std::map<animation::EHumanoidBones, std::string> SrcHumanoidBoneList;
			
			const auto humanbonelist = animationJSON->find("humanbonelist");
			if (humanbonelist != animationJSON->end() && humanbonelist->is_array())
			{
				for (json::iterator humanBoneJSON = humanbonelist->begin(); humanBoneJSON != humanbonelist->end(); humanBoneJSON++)
				{
					if (!humanBoneJSON->is_object()) continue;

					// bonename
					std::string bonename_str = std::string();
					GetString("bonename", bonename_str, humanBoneJSON);

					// nodename
					std::string nodename = std::string();
					GetString("nodename", nodename, humanBoneJSON);

					// EHumanoidBonesにキャスト
					animation::EHumanoidBones BoneName = animation::CSkeleton::CastStringToHumanoidBones(bonename_str);

					SrcHumanoidBoneList.emplace(BoneName, nodename);
				}
			}

			std::string name = "";
			GetString("name", name, animationJSON);

			std::string filename = "";
			GetString("filename", filename, animationJSON);

			std::shared_ptr<animation::CAnimationClipSet> AnimationClipSet = std::make_shared<animation::CAnimationClipSet>();

			pLoadWorker->AddLoadResource(std::make_shared<resource::CAnimationLoader>(filename, AnimationClipSet, RigType, SrcHumanoidBoneList));

			m_Target->AddAnimationClipSet(name, AnimationClipSet);
		}

		return true;
	}
#endif // USE_ANIMATION

	bool CSceneLoader::AnalyseObjects(const json::iterator& objects, api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::CApp* pApp)
	{
		for (json::iterator objectJSON = objects->begin(); objectJSON != objects->end(); objectJSON++)
		{
			if (!objectJSON->is_object()) continue;

			// renderpasslist
			std::vector<std::string> renderpasslist;
			GetArrayString("renderpasslist", renderpasslist, objectJSON);

			// C3DObject生成
			std::shared_ptr<object::C3DObject> Object = std::make_shared<object::C3DObject>();

			for (const auto& renderpass : renderpasslist)
			{
				Object->AddPassName(renderpass);
			}

			// ObjectName
			{
				std::string objname = "";
				GetString("name", objname, objectJSON);

				Object->SetObjectName(objname);
			}

			// enable
			bool enable = true;
			GetBoolean("enable", enable, objectJSON);
			Object->SetEnabled(enable);

			// Transform
			{
				auto Transform = AnalyseTransform(objectJSON);

				Object->SetPos(Transform->GetPos());
				Object->SetRot(Transform->GetRot());
				Object->SetScale(Transform->GetScale());
			}

			// rootnodes
			{
				std::vector<int> RootNodeIndexList;

				GetArrayInt32("rootnodes", RootNodeIndexList, objectJSON);

				Object->SetRootNodeIndexList(RootNodeIndexList);
			}

			// nodes
			const auto nodes = objectJSON->find("nodes");
			if (nodes != objectJSON->end() && nodes->is_array())
			{
				for (json::iterator nodeJSON = nodes->begin(); nodeJSON != nodes->end(); nodeJSON++)
				{
					if (!nodeJSON->is_object()) continue;

					std::shared_ptr<object::CNode> Node = AnalyseNode(nodeJSON, pPhysicsEngine, Object, pApp);
					Object->AddNode(Node);
				}

				Object->ApplyDefaultLocalTransform();
				Object->CalcWorldMatrix();
			}

			// meshs
			const auto meshs = objectJSON->find("meshs");
			if (meshs != objectJSON->end() && meshs->is_array())
			{
				for (json::iterator meshJSON = meshs->begin(); meshJSON != meshs->end(); meshJSON++)
				{
					if (!meshJSON->is_object()) continue;

					std::shared_ptr<graphics::CMesh> Mesh = AnalyseMesh(pGraphicsAPI, meshJSON);
					Object->AddMesh(Mesh);
				}
			}

			// materials
			const auto materials = objectJSON->find("materials");
			if (materials != objectJSON->end() && materials->is_array())
			{
				std::map<std::tuple<int, int>, std::vector<scene::SMaterialInfo>> MaterialInfoList;

				for (json::iterator materialJSON = materials->begin(); materialJSON != materials->end(); materialJSON++)
				{
					if (!materialJSON->is_object()) continue;

					scene::SMaterialInfo MaterialInfo = AnalyseMaterialInfo(materialJSON);

					std::tuple<int, int> RendererKey = std::make_tuple(MaterialInfo.MeshIndex, MaterialInfo.PrimitiveIndex);
					if (MaterialInfoList.find(RendererKey) == MaterialInfoList.end())
					{
						MaterialInfoList.emplace(RendererKey, std::vector<scene::SMaterialInfo>());
					}

					MaterialInfoList[RendererKey].push_back(MaterialInfo);
				}

				// SceneControllerに登録
				m_Target->AddMaterialInfo(Object, MaterialInfoList);
			}

#ifdef USE_TEXTURE_LOADER
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

					bool mipmap = false;
					GetBoolean("mipmap", mipmap, textureJSON);

					graphics::STextureSamplerParam SamplerParam{};

					std::string filter = std::string();
					GetString("filter", filter, textureJSON);
					if (filter == "linear")
					{
						SamplerParam.FilterMode = graphics::ETextureFilterMode::LINEAR;
					}
					else if (filter == "nearest")
					{
						SamplerParam.FilterMode = graphics::ETextureFilterMode::NEAREST;
					}

					std::string wrap = std::string();
					GetString("wrap", wrap, textureJSON);
					if (wrap == "clamptoedge")
					{
						SamplerParam.WrapMode = graphics::ETextureWrapMode::CLAMP_TO_EDGE;
					}
					else if (wrap == "repeat")
					{
						SamplerParam.WrapMode = graphics::ETextureWrapMode::REPEAT;
					}

					auto Texture = pGraphicsAPI->CreateTexture(mipmap, SamplerParam);
					pLoadWorker->AddLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, TextureFileName, Texture));

					TextureInfoList.emplace(TextureName, Texture);
				}

				m_Target->AddTextureInfo(Object, TextureInfoList);
			}
#endif // USE_TEXTURE_LOADER

			// animation
			scene::SAnimationInfo AnimationInfo{};
			const auto animationJSON = objectJSON->find("animation");
			if (animationJSON != objectJSON->end() && animationJSON->is_object())
			{
				AnimationInfo = AnalyseAnimationInfo(animationJSON);

				m_Target->AddAnimationInfo(Object, AnimationInfo);
			}

			// ファイルロード開始
			{
				std::string filename = "";
				GetString("filename", filename, objectJSON);

				std::vector<std::string> defaultmaterialframeList;
				GetArrayString("defaultmaterialframes", defaultmaterialframeList, objectJSON);

				if (!filename.empty())
				{
					std::vector<std::shared_ptr<graphics::CMaterialFrame>> BaseMaterialFrameList;

					const auto& MaterialFrameMap = m_Target->GetMaterialFrameMap();

					for (const auto& defaultmaterialframe : defaultmaterialframeList)
					{
						const auto& MaterialFrame = MaterialFrameMap.find(defaultmaterialframe);
						if (MaterialFrame == MaterialFrameMap.end())
						{
							Console::Log("[SceneLoader Error] defaultmaterialframe not found\n");

							return false;
						}

						BaseMaterialFrameList.push_back(MaterialFrame->second);
					}
					
					// 仮実装
					pLoadWorker->AddLoadResource(std::make_shared<resource::C3DObjectLoader>(filename, Object, BaseMaterialFrameList, defaultmaterialframeList, AnimationInfo.RigType, AnimationInfo.HumanoidBoneList));
				}
			}

			// Joints
			const auto joints = objectJSON->find("joints");
			if (joints != objectJSON->end() && joints->is_array())
			{
				const auto& NodeList = Object->GetNodeList();

				for (json::iterator jointJSON = joints->begin(); jointJSON != joints->end(); jointJSON++)
				{
					if (!jointJSON->is_object()) continue;

					physics::SJointParam JParam{};
					physics::EJointType JointType = physics::EJointType::NONE;

					// fixednode
					int FixedNodeIndex = -1;
					GetInt("fixednode", FixedNodeIndex, jointJSON);
					if (FixedNodeIndex < 0 || FixedNodeIndex >= static_cast<int>(NodeList.size())) continue;

					const auto& FixedNode = NodeList[FixedNodeIndex];
					if (!FixedNode) continue;

					// freenode
					int FreeNodeIndex = -1;
					GetInt("freenode", FreeNodeIndex, jointJSON);
					if (FreeNodeIndex < 0 || FreeNodeIndex >= static_cast<int>(NodeList.size())) continue;

					const auto& FreeNode = NodeList[FreeNodeIndex];
					if (!FreeNode) continue;

					// jointtype
					std::string JointTypeStr = std::string();
					GetString("jointtype", JointTypeStr, jointJSON);

					if (JointTypeStr == "spring_6dof")
					{
						JointType = physics::EJointType::SPRING_6DOF;
					}
					else if (JointTypeStr == "generic_6dof")
					{
						JointType = physics::EJointType::Generic_6DOF;
					}
					else if (JointTypeStr == "p2p")
					{
						JointType = physics::EJointType::P2P;
					}
					else if (JointTypeStr == "cone_twist")
					{
						JointType = physics::EJointType::ConeTwist;
					}
					else if (JointTypeStr == "slider")
					{
						JointType = physics::EJointType::Slider;
					}

					// jointpos
					std::vector<float> jointposArray;
					GetArrayFloat32("jointpos", jointposArray, jointJSON);
					
					glm::vec3 jointpos = glm::vec3(0.0f);
					if (jointposArray.size() == 3)
					{
						jointpos.x = jointposArray[0];
						jointpos.y = jointposArray[1];
						jointpos.z = jointposArray[2];
					}

					// jointpos
					{
						std::vector<float> jointposArray;
						GetArrayFloat32("jointpos", jointposArray, jointJSON);

						if (jointposArray.size() == 3)
						{
							JParam.JointPos.x = jointposArray[0];
							JParam.JointPos.y = jointposArray[1];
							JParam.JointPos.z = jointposArray[2];
						}
					}

					// jointrotate
					{
						std::vector<float> jointrotateArray;
						GetArrayFloat32("jointrotate", jointrotateArray, jointJSON);

						if (jointrotateArray.size() == 3)
						{
							JParam.JointRotate.x = jointrotateArray[0];
							JParam.JointRotate.y = jointrotateArray[1];
							JParam.JointRotate.z = jointrotateArray[2];
						}
					}

					// lowertranslimit
					{
						std::vector<float> lowertranslimitArray;
						GetArrayFloat32("lowertranslimit", lowertranslimitArray, jointJSON);

						if (lowertranslimitArray.size() == 3)
						{
							JParam.LowerTransLimit.x = lowertranslimitArray[0];
							JParam.LowerTransLimit.y = lowertranslimitArray[1];
							JParam.LowerTransLimit.z = lowertranslimitArray[2];
						}
					}

					// uppertranslimit
					{
						std::vector<float> uppertranslimitArray;
						GetArrayFloat32("uppertranslimit", uppertranslimitArray, jointJSON);

						if (uppertranslimitArray.size() == 3)
						{
							JParam.UpperTransLimit.x = uppertranslimitArray[0];
							JParam.UpperTransLimit.y = uppertranslimitArray[1];
							JParam.UpperTransLimit.z = uppertranslimitArray[2];
						}
					}

					// lowerrotatelimit
					{
						std::vector<float> lowerrotatelimitArray;
						GetArrayFloat32("lowerrotatelimit", lowerrotatelimitArray, jointJSON);

						if (lowerrotatelimitArray.size() == 3)
						{
							JParam.LowerRotateLimit.x = lowerrotatelimitArray[0];
							JParam.LowerRotateLimit.y = lowerrotatelimitArray[1];
							JParam.LowerRotateLimit.z = lowerrotatelimitArray[2];
						}
					}

					// upperrotatelimit
					{
						std::vector<float> upperrotatelimitArray;
						GetArrayFloat32("upperrotatelimit", upperrotatelimitArray, jointJSON);

						if (upperrotatelimitArray.size() == 3)
						{
							JParam.UpperRotateLimit.x = upperrotatelimitArray[0];
							JParam.UpperRotateLimit.y = upperrotatelimitArray[1];
							JParam.UpperRotateLimit.z = upperrotatelimitArray[2];
						}
					}

					// transspring
					{
						std::vector<float> transspringArray;
						GetArrayFloat32("transspring", transspringArray, jointJSON);

						if (transspringArray.size() == 3)
						{
							JParam.TransSpring.x = transspringArray[0];
							JParam.TransSpring.y = transspringArray[1];
							JParam.TransSpring.z = transspringArray[2];
						}
					}

					// rotatespring
					{
						std::vector<float> rotatespringArray;
						GetArrayFloat32("rotatespring", rotatespringArray, jointJSON);

						if (rotatespringArray.size() == 3)
						{
							JParam.RotateSpring.x = rotatespringArray[0];
							JParam.RotateSpring.y = rotatespringArray[1];
							JParam.RotateSpring.z = rotatespringArray[2];
						}
					}

					// ジョイントを予約
					if (JointType != physics::EJointType::NONE && !FixedNode->GetPhysicsObjectList().empty() && !FreeNode->GetPhysicsObjectList().empty())
					{
						// シーン側では単一の物理オブジェクトしか考慮していない
						const auto& FixedPhysicsObject = FixedNode->GetPhysicsObjectList()[0];
						const auto& FreePhysicsObject = FreeNode->GetPhysicsObjectList()[0];

						FreePhysicsObject->ReserveConstraint(FixedPhysicsObject, JointType, JParam);
					}
				}
			}

			// Objectを追加
			m_Target->AddObject(Object);
		}

		return true;
	}

	std::shared_ptr<object::CNode> CSceneLoader::AnalyseNode(const json::iterator& nodeJSON, physics::IPhysicsEngine* pPhysicsEngine, const std::shared_ptr<object::C3DObject>& Object, app::CApp* pApp)
	{
		std::string nodename = "";
		GetString("name", nodename, nodeJSON);

		auto Transform = AnalyseTransform(nodeJSON);

		// enable
		bool enable = true;
		GetBoolean("enable", enable, nodeJSON);

		int meshindex = -1;
		GetInt("meshindex", meshindex, nodeJSON);

		std::vector<int> children;
		GetArrayInt32("children", children, nodeJSON);

		std::vector<std::string> trackids;
		GetArrayString("trackids", trackids, nodeJSON);

		// ノードを作成
		int SelfNodeIndex = static_cast<int>(Object->GetNodeList().size());
		std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(meshindex, SelfNodeIndex);

		// コンポーネント
		const auto components = nodeJSON->find("components");
		if (components != nodeJSON->end() && components->is_array())
		{
			for (json::iterator componentJSON = components->begin(); componentJSON != components->end(); componentJSON++)
			{
				std::string type = std::string();
				GetString("type", type, componentJSON);

				std::string valueregistry = std::string();
				GetString("valueregistry", valueregistry, componentJSON);

				// コンポーネントを作成
				auto Component = pApp->CreateComponent(type, valueregistry);
				if (Component)
				{
					Node->AddComponent(Component);
				}
			}
		}

		// Collider
		const auto collider = nodeJSON->find("collider");
		std::string colliderShape = std::string();
		std::vector<float> colliderSize;
		if (collider != nodeJSON->end() && collider->is_object())
		{
			GetString("shape", colliderShape, collider);

			GetArrayFloat32("size", colliderSize, collider);
		}

		// RigidBody
		const auto rigidbody = nodeJSON->find("rigidbody");
		if (rigidbody != nodeJSON->end() && rigidbody->is_object())
		{
			// 物理パラメーター
			physics::SRigidbodyParam RBParam{};
			
			// mass
			float mass = 0.0f;
			GetFloat("mass", mass, rigidbody);
			RBParam.Mass = mass;

			// physicstype
			std::string PhysicsTypeStr = std::string();
			GetString("physicstype", PhysicsTypeStr, rigidbody);
			
			if (PhysicsTypeStr == "static")
			{
				RBParam.PhysicsType = physics::EPhysicsType::STATIC;
			}
			else if (PhysicsTypeStr == "dynamic")
			{
				RBParam.PhysicsType = physics::EPhysicsType::DYNAMIC;
			}
			else if (PhysicsTypeStr == "dynamic_joint")
			{
				RBParam.PhysicsType = physics::EPhysicsType::DYNAMIC_JOINT;
			}

			// transdamping
			float transdamping = 0.0f;
			GetFloat("transdamping", transdamping, rigidbody);
			RBParam.TransDamping = transdamping;

			// rotatedamping
			float rotatedamping = 0.0f;
			GetFloat("rotatedamping", rotatedamping, rigidbody);
			RBParam.RotateDamping = rotatedamping;

			// repulsion
			float repulsion = 0.0f;
			GetFloat("repulsion", repulsion, rigidbody);
			RBParam.Repulsion = repulsion;

			// friction
			float friction = 0.5f;
			GetFloat("friction", friction, rigidbody);
			RBParam.Friction = friction;

			// 物理オブジェクトを生成
			std::shared_ptr<physics::IPhysicsObject> PhysicsObject = nullptr;

			if (colliderShape == "box")
			{
				glm::vec3 PhysicsSize = glm::vec3(1.0f);
				if (colliderSize.size() == 3)
				{
					PhysicsSize.x = colliderSize[0];
					PhysicsSize.y = colliderSize[1];
					PhysicsSize.z = colliderSize[2];
				}

				PhysicsObject = pPhysicsEngine->CreatePhysicsBox(PhysicsSize, (RBParam.PhysicsType == physics::EPhysicsType::STATIC), RBParam);
			}
			else if (colliderShape == "sphere")
			{
				float PhysicsSize = 1.0f;
				if (colliderSize.size() == 1)
				{
					PhysicsSize = colliderSize[0];
				}

				PhysicsObject = pPhysicsEngine->CreatePhysicsSphere(PhysicsSize, (RBParam.PhysicsType == physics::EPhysicsType::STATIC), RBParam);
			}
			else if (colliderShape == "capsule")
			{
				glm::vec2 PhysicsSize = glm::vec2(1.0f);
				if (colliderSize.size() == 2)
				{
					PhysicsSize.x = colliderSize[0];
					PhysicsSize.y = colliderSize[1];
				}

				PhysicsObject = pPhysicsEngine->CreatePhysicsCapsule(PhysicsSize.x, PhysicsSize.y, (RBParam.PhysicsType == physics::EPhysicsType::STATIC), RBParam);
			}

			if (PhysicsObject)
			{
				Node->AddPhysicsObject(PhysicsObject);
			}
		}

		//
		Node->SetEnabled(enable);
		Node->SetName(nodename);
		Node->SetLocalTransform(Transform);
		Node->SetChildrenNodeIndexList(children);
		Node->SetRefTrackIDList(trackids);

		return Node;
	}

	std::shared_ptr<graphics::CMesh> CSceneLoader::AnalyseMesh(api::IGraphicsAPI* pGraphicsAPI, const json::iterator& meshJSON)
	{
		std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

		const auto primitives = meshJSON->find("primitives");
		if (primitives != meshJSON->end() && primitives->is_array())
		{
			for (json::iterator primitiveJSON = primitives->begin(); primitiveJSON != primitives->end(); primitiveJSON++)
			{
				if (!primitiveJSON->is_object()) continue;

				std::string type = "";
				GetString("type", type, primitiveJSON);

				graphics::EPresetPrimitiveType PrimitiveType = graphics::EPresetPrimitiveType::None;

				std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> createInfo;

				if (type == "cube")
				{
					createInfo = graphics::CPresetPrimitive::CreateBox(pGraphicsAPI);
					PrimitiveType = graphics::EPresetPrimitiveType::CUBE;
				}
				else if (type == "board" || type == "plane")
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
				Mesh->CreatePresetSimpleMesh(pGraphicsAPI, createInfo.first, createInfo.second, nullptr, PrimitiveType);
			}
		}

		return Mesh;
	}

	scene::SMaterialInfo CSceneLoader::AnalyseMaterialInfo(const json::iterator& materialJSON)
	{
		scene::SMaterialInfo MaterialInfo{};

		// マテリアルフレーム名
		std::string materialframe = "";
		GetString("materialframe", materialframe, materialJSON);
		MaterialInfo.MaterialFrameName = materialframe;

		// メッシュインデックス
		int meshindex = -1;
		GetInt("meshindex", meshindex, materialJSON);
		MaterialInfo.MeshIndex = meshindex;

		// プリミティブインデックス
		int primitiveindex = -1;
		GetInt("primitiveindex", primitiveindex, materialJSON);
		MaterialInfo.PrimitiveIndex = primitiveindex;

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

		// TrackID
		{
			std::vector<std::string> trackids;
			GetArrayString("trackids", trackids, materialJSON);
			MaterialInfo.TrackIDList = trackids;
		}

		return MaterialInfo;
	}

	scene::SAnimationInfo CSceneLoader::AnalyseAnimationInfo(const json::iterator& animationJSON)
	{
		scene::SAnimationInfo AnimationInfo{};

		// humanoid
		std::string rig = std::string();
		GetString("rig", rig, animationJSON);

		if (rig == "humanoid")
		{
			AnimationInfo.RigType = animation::ERigType::Humanoid;
		}

		// humanbonelist
		std::map<animation::EHumanoidBones, std::string> SrcHumanoidBoneList;

		const auto humanbonelist = animationJSON->find("humanbonelist");
		if (humanbonelist != animationJSON->end() && humanbonelist->is_array())
		{
			for (json::iterator humanBoneJSON = humanbonelist->begin(); humanBoneJSON != humanbonelist->end(); humanBoneJSON++)
			{
				if (!humanBoneJSON->is_object()) continue;

				// bonename
				std::string bonename_str = std::string();
				GetString("bonename", bonename_str, humanBoneJSON);

				// nodename
				std::string nodename = std::string();
				GetString("nodename", nodename, humanBoneJSON);

				// EHumanoidBonesにキャスト
				animation::EHumanoidBones BoneName = animation::CSkeleton::CastStringToHumanoidBones(bonename_str);
				if (BoneName == animation::EHumanoidBones::None) continue;

				SrcHumanoidBoneList.emplace(BoneName, nodename);
			}
		}

		AnimationInfo.HumanoidBoneList = SrcHumanoidBoneList;

		// clips
		const auto clips = animationJSON->find("clips");
		if (clips != animationJSON->end() && clips->is_array())
		{
			for (json::iterator clipJSON = clips->begin(); clipJSON != clips->end(); clipJSON++)
			{
				if (!clipJSON->is_object()) continue;

				scene::SAnimationClip Clip{};

				std::string key = "";
				GetString("key", key, clipJSON);
				Clip.Key = key;

				std::string motionname = "";
				GetString("motionname", motionname, clipJSON);
				Clip.MotionName = motionname;

				int index = -1;
				GetInt("index", index, clipJSON);
				Clip.Index = index;

				bool loop = false;
				GetBoolean("loop", loop, clipJSON);
				Clip.Loop = loop;

				AnimationInfo.Clips.emplace(key, Clip);
			}
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

				AnimationInfo.Humanoidclips.emplace(key, Clip);
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

				AnimationInfo.Blendshapes.emplace(key, Clip);
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

		auto transformJSON = Object->find("transform");
		if (transformJSON != Object->end() && transformJSON->is_object())
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

	void CSceneLoader::GetArrayString(const std::string& Key, std::vector<std::string>& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_array())
		{
			for (json::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				if (it2->is_string())
				{
					std::string val = it2.value();

					Value.push_back(val);
				}
			}
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