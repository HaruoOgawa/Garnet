#include "CScriptScene.h"

#include "../LoadWorker/CLoadWorker.h"
#include "../LoadWorker/CFile.h"
#include "../LoadWorker/CMaterialFrameLoader.h"
#include "../LoadWorker/CTextureLoader.h"
#include "../LoadWorker/C3DObjectLoader.h"
#include "../LoadWorker/CAnimationLoader.h"
#include "../LoadWorker/CAudioLoader.h"

#include "../Message/Console.h"

#include "../Object/C3DObject.h"
#include "../Animation/CAnimationClipSet.h"
#include "../Graphics/CMaterialFrame.h"

namespace scene
{
	CScriptScene::CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker):
		m_BasicToonMF(std::make_shared<graphics::CMaterialFrame>()),
		m_SimpleTextureMF(std::make_shared<graphics::CMaterialFrame>()),
		m_DepthMF(std::make_shared<graphics::CMaterialFrame>()),
		m_PBRMF(std::make_shared<graphics::CMaterialFrame>()),

		m_AudioClip(std::make_shared<audio::CAudioClip>()),

		m_SimpleMorphObj(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_BrainStemObj(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_Walk_Animation(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_Jump_Animation(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_Punch_Animation(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_XBotObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_MouseyObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_WarrokObject(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_PhysicsGround(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_PhysicsSphere(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_PhysicsCubeList(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_TdaMiku_Model(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_VMDAnimationSet(std::make_shared<animation::CAnimationClipSet>()),
		m_ExpressionVMD(std::make_shared<animation::CAnimationClipSet>()),
		m_RipSyncVMD(std::make_shared<animation::CAnimationClipSet>()),

		m_Background(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_IBL_Skybox_Texture(pGraphicsAPI->CreateTexture(false)),
		m_Cube_Texture(pGraphicsAPI->CreateTexture(false)),
		m_DebugSphere(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_IsLoaded(false)
	{
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\basic_toon_mf.json", m_BasicToonMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\SimpleTexture_MF.json", m_SimpleTextureMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\Depth_MF.json", m_DepthMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\PBR_MF.json", m_PBRMF));
		
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Models\\SimpleMorphTarget\\SimpleMorphTarget.gltf", m_SimpleMorphObj, "", "ShadowPass"));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Models\\MorphPrimitivesTest\\glTF-Binary\\MorphPrimitivesTest.glb", m_SimpleMorphObj, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Models\\MorphStressTest\\glTF-Binary\\MorphStressTest.glb", m_SimpleMorphObj, "", "ShadowPass"));
		/*pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Models\\BrainStem\\glTF-Binary\\BrainStem.glb", m_BrainStemObj, "", "ShadowPass"));

		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Avatar\\X_Bot.fbx", m_XBotObject, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Avatar\\Mousey.fbx", m_MouseyObject, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Avatar\\Warrok_W_Kurniawan.fbx", m_WarrokObject, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Motions\\Walking.fbx", m_Walk_Animation, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Motions\\Jumping.fbx", m_Jump_Animation, "", "ShadowPass"));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Motions\\Combo_Punch_hand.fbx", m_Punch_Animation, "", "ShadowPass"));*/

		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Avatar\\Tda_Miku\\Tda_Miku.pmx", m_TdaMiku_Model, "", "ShadowPass"));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\mmd_running.vmd", m_VMDAnimationSet));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\Run_m4th_Loop.vmd", m_VMDAnimationSet));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\Bling-Bang-Bang-Born.vmd", m_VMDAnimationSet));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\biglove_dance.vmd", m_VMDAnimationSet));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\BackFlip.vmd", m_VMDAnimationSet));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\biglove_expression.vmd", m_ExpressionVMD));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\biglove_Ripsync.vmd", m_RipSyncVMD));
		
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\IBL\\output_skybox.hdr" }), m_IBL_Skybox_Texture));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\Cubemaps\\environment\\environment_back_0.jpg", "Resources\\Cubemaps\\environment\\environment_bottom_0.jpg", "Resources\\Cubemaps\\environment\\environment_front_0.jpg", "Resources\\Cubemaps\\environment\\environment_left_0.jpg", "Resources\\Cubemaps\\environment\\environment_right_0.jpg", "Resources\\Cubemaps\\environment\\environment_top_0.jpg" }), m_Cube_Texture));
	
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAudioLoader>("Resources\\Audio\\biglove_audio.wav", m_AudioClip));
	}

	CScriptScene::~CScriptScene()
	{
		m_IsLoaded = false;
	}

	bool CScriptScene::IsLoaded() const
	{
		return m_IsLoaded;
	}

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		// m_SimpleMorphObj
		/*{
			if (!m_SimpleMorphObj->CreateFromMemory(pGraphicsAPI, pPhysicsEngine, pLoadWorker, m_PBRMF, m_DepthMF)) return false;
			m_SimpleMorphObj->ChangeMotion(0);
		}*/
		
		// m_BrainStemObj
		/*{
			if (!m_BrainStemObj->CreateFromMemory(pGraphicsAPI, pPhysicsEngine, pLoadWorker, m_PBRMF, m_DepthMF)) return false;
			m_BrainStemObj->ChangeMotion(0);
		}

		// FBX Humanoid Animation Clip
		std::vector<std::shared_ptr<animation::CAnimationClip>> AnimationClipList;
		{
			if (!m_Walk_Animation->CreateFromMemory(pGraphicsAPI, pPhysicsEngine, pLoadWorker, nullptr, nullptr)) return false;
			if (!m_Jump_Animation->CreateFromMemory(pGraphicsAPI, pPhysicsEngine, pLoadWorker, nullptr, nullptr)) return false;
			if (!m_Punch_Animation->CreateFromMemory(pGraphicsAPI, pPhysicsEngine, pLoadWorker, nullptr, nullptr)) return false;

			AnimationClipList.push_back(m_Walk_Animation->GetAnimationClipList()[0]);
			AnimationClipList.push_back(m_Jump_Animation->GetAnimationClipList()[0]);
			AnimationClipList.push_back(m_Punch_Animation->GetAnimationClipList()[0]);
		}

		{
			if (!m_XBotObject->CreateFromMemory(pGraphicsAPI, pPhysicsEngine, pLoadWorker, m_PBRMF, m_DepthMF)) return false;

			m_XBotObject->AddHumanoidAnimationClip(AnimationClipList[0], "Walk", { nullptr, "" }, true);
			m_XBotObject->AddHumanoidAnimationClip(AnimationClipList[1], "Jump", { nullptr, "Walk" }, false);
			m_XBotObject->AddHumanoidAnimationClip(AnimationClipList[2], "Punch", { nullptr, "Walk" }, false);

			// 再生するアニメーションクリップを指定する
			m_XBotObject->ChangeMotion("Walk");
			m_XBotObject->SetPos(glm::vec3(0.0f, 0.0f, 1.0f));
			m_XBotObject->SetRot(glm::angleAxis(3.1515f, glm::vec3(0.0f, 1.0f, 0.0f)));
			m_XBotObject->SetScale(glm::vec3(0.01f));
		}

		{
			if (!m_MouseyObject->CreateFromMemory(pGraphicsAPI, pPhysicsEngine, pLoadWorker, m_PBRMF, m_DepthMF)) return false;

			m_MouseyObject->AddHumanoidAnimationClip(AnimationClipList[0], "Walk", { nullptr, "" }, true);
			m_MouseyObject->AddHumanoidAnimationClip(AnimationClipList[1], "Jump", { nullptr, "Walk" }, false);
			m_MouseyObject->AddHumanoidAnimationClip(AnimationClipList[2], "Punch", { nullptr, "Walk" }, false);

			// 再生するアニメーションクリップを指定する
			m_MouseyObject->ChangeMotion("Walk");

			m_MouseyObject->SetPos(glm::vec3(-2.0f, 0.0f, 0.0f));
			m_MouseyObject->SetRot(glm::angleAxis(3.1515f, glm::vec3(0.0f, 1.0f, 0.0f)));
			m_MouseyObject->SetScale(glm::vec3(0.01f));
		}

		{
			if (!m_WarrokObject->CreateFromMemory(pGraphicsAPI, pPhysicsEngine, pLoadWorker, m_PBRMF, m_DepthMF)) return false;

			m_WarrokObject->AddHumanoidAnimationClip(AnimationClipList[0], "Walk", { nullptr, "" }, true);
			m_WarrokObject->AddHumanoidAnimationClip(AnimationClipList[1], "Jump", { nullptr, "Walk" }, false);
			m_WarrokObject->AddHumanoidAnimationClip(AnimationClipList[2], "Punch", { nullptr, "Walk" }, false);

			m_WarrokObject->SetPos(glm::vec3(-4.0f, 0.0f, 0.0f));
			m_WarrokObject->SetRot(glm::angleAxis(3.1515f, glm::vec3(0.0f, 1.0f, 0.0f)));
			m_WarrokObject->SetScale(glm::vec3(0.01f));

			// 再生するアニメーションクリップを指定する
			m_WarrokObject->ChangeMotion("Walk");
		}*/

		const float ZOffset = 3.0f;

		// m_PhysicsGround
		/*{
			auto Material = m_PBRMF->CreateMaterial(pGraphicsAPI, 1, graphics::ECullMode::CULL_BACK);

			//m_PhysicsGround->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			//Material->ReplacePreloadUniformValue("useCubeMap", &glm::ivec1(1)[0], sizeof(glm::ivec1), 0);
			//Material->ReplaceTextureIndex("cubemapTexture", 0);
			//Material->ReplacePreloadUniformValue("roughnessFactor", &glm::vec1(1.0f)[0], sizeof(float), 0);

			std::shared_ptr<math::CTransform> LocalTransform = std::make_shared<math::CTransform>();
			LocalTransform->SetPos(glm::vec3(0.0f, 0.1f, 0.0f));
			LocalTransform->SetScale(glm::vec3(10.0f, 0.1f, 10.0f));

			auto PhysicsBox = pPhysicsEngine->CreatePhysicsBox(glm::vec3(0.5f), false, 0.0f, {});

			if (!m_PhysicsGround->CreateSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateBox(pGraphicsAPI), Material, m_DepthMF, LocalTransform, PhysicsBox)) return false;
		}*/

		// m_PhysicsSphere
		/*{
			// Material
			auto Material = m_PBRMF->CreateMaterial(pGraphicsAPI, 5, graphics::ECullMode::CULL_BACK);

			m_PhysicsSphere->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			Material->ReplacePreloadUniformValue("useCubeMap", &glm::ivec1(1)[0], sizeof(glm::ivec1), 0);
			Material->ReplaceTextureIndex("cubemapTexture", 0);
			Material->ReplacePreloadUniformValue("roughnessFactor", &glm::vec1(1.0f)[0], sizeof(float), 0);

			m_PhysicsSphere->AddMaterial(Material);

			// Mesh
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
			//Mesh->CreateSimpleMesh(graphics::CPresetPrimitive::CreateSphere(pGraphicsAPI), 0);
			auto VertexIndexPair = graphics::CPresetPrimitive::CreateBox(pGraphicsAPI);
			Mesh->CreateSimpleMesh(VertexIndexPair.first, VertexIndexPair.second, 0);

			m_PhysicsSphere->AddMesh(Mesh);

			// Physics
			physics::SRigidbodyParam RbParam = {};
			RbParam.group = 32;
			//RbParam.NoneCollideGroupFlag = 161; // 1,6,8に当たらないようにする。6は自分たちのグループなのでお互いがぶつからないようにする => 10100001b
			RbParam.NoneCollideGroupFlag = 32; 
			RbParam.PhysicsType = physics::EPhysicsType::STATIC;
			auto PhysicsSphere0 = pPhysicsEngine->CreatePhysicsSphere(1.0f, true, 0.0f, RbParam);

			RbParam.PhysicsType = physics::EPhysicsType::DYNAMIC;
			auto PhysicsSphere1 = pPhysicsEngine->CreatePhysicsSphere(1.0f, false, 50.0f, RbParam);
			auto PhysicsSphere2 = pPhysicsEngine->CreatePhysicsSphere(1.0f, false, 50.0f, RbParam);
			auto PhysicsSphere3 = pPhysicsEngine->CreatePhysicsSphere(1.0f, false, 50.0f, RbParam);
			auto PhysicsSphere4 = pPhysicsEngine->CreatePhysicsSphere(1.0f, false, 50.0f, RbParam);

			// Node
			{
				std::shared_ptr<math::CTransform> LocalTransform = std::make_shared<math::CTransform>();
				LocalTransform->SetPos(glm::vec3(0.0f, 1.5f, 0.0f + ZOffset));
				LocalTransform->SetScale(glm::vec3(0.5f));

				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, -1);
				Node->SetLocalTransform(LocalTransform);
				Node->AddPhysicsObject(PhysicsSphere0);

				m_PhysicsSphere->AddNode(Node);
			}

			{
				std::shared_ptr<math::CTransform> LocalTransform = std::make_shared<math::CTransform>();
				LocalTransform->SetPos(glm::vec3(0.5, 1.5f, 0.0f + ZOffset));
				LocalTransform->SetScale(glm::vec3(0.5f));

				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, -1);
				Node->SetLocalTransform(LocalTransform);
				Node->AddPhysicsObject(PhysicsSphere1);

				m_PhysicsSphere->AddNode(Node);
			}

			{
				std::shared_ptr<math::CTransform> LocalTransform = std::make_shared<math::CTransform>();
				LocalTransform->SetPos(glm::vec3(1.0f, 1.5f, 0.0f + ZOffset));
				LocalTransform->SetScale(glm::vec3(0.5f));

				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, -1);
				Node->SetLocalTransform(LocalTransform);
				Node->AddPhysicsObject(PhysicsSphere2);

				m_PhysicsSphere->AddNode(Node);
			}

			{
				std::shared_ptr<math::CTransform> LocalTransform = std::make_shared<math::CTransform>();
				LocalTransform->SetPos(glm::vec3(1.5f, 1.5f, 0.0f + ZOffset));
				LocalTransform->SetScale(glm::vec3(0.5f));

				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, -1);
				Node->SetLocalTransform(LocalTransform);
				Node->AddPhysicsObject(PhysicsSphere3);

				m_PhysicsSphere->AddNode(Node);
			}

			{
				std::shared_ptr<math::CTransform> LocalTransform = std::make_shared<math::CTransform>();
				LocalTransform->SetPos(glm::vec3(2.0f, 1.5f, 0.0f + ZOffset));
				LocalTransform->SetScale(glm::vec3(0.5f));

				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, -1);
				Node->SetLocalTransform(LocalTransform);
				Node->AddPhysicsObject(PhysicsSphere4);

				m_PhysicsSphere->AddNode(Node);
			}
			
			// Constraintを予約する
			{
				physics::SJointParam JParam = {};
				JParam.JointPos = m_PhysicsSphere->GetNodeList()[1]->GetPos();

				PhysicsSphere1->ReserveConstraint(PhysicsSphere0, physics::EJointType::SPRING_6DOF, JParam);
			}
			
			{
				physics::SJointParam JParam = {};
				JParam.JointPos = m_PhysicsSphere->GetNodeList()[2]->GetPos();
				PhysicsSphere2->ReserveConstraint(PhysicsSphere1, physics::EJointType::SPRING_6DOF, JParam);
			}
			
			{
				physics::SJointParam JParam = {};
				JParam.JointPos = m_PhysicsSphere->GetNodeList()[3]->GetPos();
				PhysicsSphere3->ReserveConstraint(PhysicsSphere2, physics::EJointType::SPRING_6DOF, JParam);
			}
			
			{
				physics::SJointParam JParam = {};
				JParam.JointPos = m_PhysicsSphere->GetNodeList()[4]->GetPos();
				PhysicsSphere4->ReserveConstraint(PhysicsSphere3, physics::EJointType::SPRING_6DOF, JParam);
			}

			// Create
			if (!m_PhysicsSphere->Create(pGraphicsAPI, pPhysicsEngine, m_DepthMF)) return false;
		}

		// m_PhysicsCubeList
		{
			// 
			const float XMax = 4.0f;
			const float YMax = 4.0f;
			const float ZMax = 4.0f;
			const float NumOfBox = XMax * YMax * ZMax;
			const float size = 0.25f;

			// Material
			auto Material = m_PBRMF->CreateMaterial(pGraphicsAPI, static_cast<int>(NumOfBox), graphics::ECullMode::CULL_BACK);

			m_PhysicsCubeList->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			Material->ReplacePreloadUniformValue("useCubeMap", &glm::ivec1(1)[0], sizeof(glm::ivec1), 0);
			Material->ReplaceTextureIndex("cubemapTexture", 0);
			Material->ReplacePreloadUniformValue("roughnessFactor", &glm::vec1(1.0f)[0], sizeof(float), 0);

			m_PhysicsCubeList->AddMaterial(Material);

			// Mesh
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
			auto VertexIndexPair = graphics::CPresetPrimitive::CreateBox(pGraphicsAPI);
			Mesh->CreateSimpleMesh(VertexIndexPair.first, VertexIndexPair.second, 0);

			m_PhysicsCubeList->AddMesh(Mesh);

			// Node
			for (float z = 0.0f; z < ZMax; z++)
			{
				for (float y = 0.0f; y < YMax; y++)
				{
					for (float x = 0.0f; x < XMax; x++)
					{
						float xpos = x * 2.0f - XMax;
						float ypos = y + size;
						float zpos = z * 2.0f - ZMax;

						std::shared_ptr<math::CTransform> LocalTransform = std::make_shared<math::CTransform>();
						LocalTransform->SetPos(glm::vec3(xpos * size * 0.5f, ypos * size * 0.5f, zpos * size * 0.5f + ZOffset));
						LocalTransform->SetScale(glm::vec3(size));

						physics::SRigidbodyParam RbParam = {};
						RbParam.group = 128;
						//RbParam.NoneCollideGroupFlag = 32; // 6個目のビットを立てる → 6と当たらないようにする => 00100000b

						RbParam.PhysicsType = physics::EPhysicsType::DYNAMIC;

						auto PhysicsBox = pPhysicsEngine->CreatePhysicsBox(glm::vec3(0.5f), false, 1.0f, RbParam);

						std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, -1);
						Node->SetLocalTransform(LocalTransform);
						Node->AddPhysicsObject(PhysicsBox);

						m_PhysicsCubeList->AddNode(Node);
					}
				}
			}

			// Create
			if (!m_PhysicsCubeList->Create(pGraphicsAPI, pPhysicsEngine, m_DepthMF)) return false;
		}*/

		// m_TdaMiku_Model
		{
			m_TdaMiku_Model->SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
			m_TdaMiku_Model->SetScale(glm::vec3(0.1f));

			if (!m_TdaMiku_Model->CreateFromMemory(pGraphicsAPI, pPhysicsEngine, pLoadWorker, m_BasicToonMF, m_DepthMF)) return false;

			auto Clip = m_VMDAnimationSet->GetAnimationClip(0);
			if (Clip) m_TdaMiku_Model->AddHumanoidAnimationClip(Clip, "Walk", { nullptr, "" }, true);

			//auto ExpressionClip = m_VMDAnimationSet->GetBlendShapeClip(0);
			auto ExpressionClip = m_ExpressionVMD->GetBlendShapeClip(0);
			if (ExpressionClip) m_TdaMiku_Model->AddBlendShapeClip(ExpressionClip, "Face", true);

			auto RipSyncClip = m_RipSyncVMD->GetBlendShapeClip(0);
			if (RipSyncClip) m_TdaMiku_Model->AddBlendShapeClip(RipSyncClip, "RipSync", true);

			m_TdaMiku_Model->ChangeMotion("Walk");
			m_TdaMiku_Model->PlayBlendShape("Face");
			m_TdaMiku_Model->PlayBlendShape("RipSync");
		}

		// m_Background
		{
			auto Mat = m_SimpleTextureMF->CreateMaterial(pGraphicsAPI, 1, graphics::ECullMode::CULL_FRONT);
			Mat->ReplacePreloadUniformValue("useDirSampling", &glm::ivec1(1)[0], sizeof(glm::ivec1), 1);
			Mat->ReplacePreloadUniformValue("useTexColor", &glm::ivec1(1)[0], sizeof(glm::ivec1), 1);
			Mat->ReplaceTextureIndex("texImage", 0);

			m_Background->GetTextureSet()->Add2DTexture(m_IBL_Skybox_Texture);

			m_Background->SetScale(glm::vec3(500.0f));
			if (!m_Background->CreateSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateSphere(pGraphicsAPI), Mat , m_DepthMF)) return false;
		}

		// m_DebugSphere
		{
			auto Mat = m_PBRMF->CreateMaterial(pGraphicsAPI, 512, graphics::ECullMode::CULL_BACK);
			Mat->SetEnabledZTest(false);
			m_DebugSphere->SetScale(glm::vec3(0.1f));
			if (!m_DebugSphere->CreateSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateSphere(pGraphicsAPI), Mat, m_DepthMF)) return false;
		}

		if (!m_AudioClip->PlayLoop()) return false;

		return true;
	}

#ifdef USE_INPUT_SYSTEM
	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
#else
	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
#endif
	{
		if (!m_IsLoaded)
		{
			if (!pLoadWorker->IsLoaded()) return true;

			if (!Load(pGraphicsAPI, pPhysicsEngine, pLoadWorker)) return false;
			m_IsLoaded = true;
		}

		if (InputState->IsKeyUp(input::EKeyType::KEY_TYPE_1))
		{
			m_XBotObject->ChangeMotion("Jump");
			m_MouseyObject->ChangeMotion("Jump");
			m_WarrokObject->ChangeMotion("Jump");
		}
		else if (InputState->IsKeyUp(input::EKeyType::KEY_TYPE_2))
		{
			m_XBotObject->ChangeMotion("Punch");
			m_MouseyObject->ChangeMotion("Punch");
			m_WarrokObject->ChangeMotion("Punch");
		}

		if (m_SimpleMorphObj)
		{
			if (!m_SimpleMorphObj->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_BrainStemObj)
		{
			if (!m_BrainStemObj->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		if (m_WarrokObject)
		{
			if (!m_WarrokObject->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		if (m_XBotObject)
		{
			if (!m_XBotObject->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		if (m_MouseyObject)
		{
			if (!m_MouseyObject->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_PhysicsGround)
		{
			if (!m_PhysicsGround->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_PhysicsSphere)
		{
			const float ZOffset = 3.0f;
			//m_PhysicsSphere->GetNodeList()[0]->SetPos(glm::vec3(glm::sin(DrawInfo->GetSecondsTime()), 1.5f, 0.0f + ZOffset));
			//m_PhysicsSphere->GetNodeList()[0]->SetRot(glm::angleAxis(glm::sin(DrawInfo->GetSecondsTime()) * 3.1415f + 3.1415f, glm::vec3(0.0f, 0.0f, 1.0f)));

			if (!m_PhysicsSphere->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_PhysicsCubeList)
		{
			if (!m_PhysicsCubeList->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		if (m_TdaMiku_Model)
		{
			if (!m_TdaMiku_Model->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;

			if (m_TdaMiku_Model->GetNodeList().size() > 0)
			{
				//m_PhysicsSphere->GetNodeList()[0]->SetWorldMatrix(m_TdaMiku_Model->GetNodeList()[110]->GetWorldMatrix());
			}
		}
		
		if (m_Background)
		{
			if (!m_Background->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		if (m_DebugSphere)
		{
			if (!m_DebugSphere->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}

	bool CScriptScene::LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		return true;
	}

	bool CScriptScene::FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		if (m_PhysicsGround)
		{
			if (!m_PhysicsGround->FixedUpdate(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		if (m_PhysicsSphere)
		{
			if (!m_PhysicsSphere->FixedUpdate(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		if (m_PhysicsCubeList)
		{
			if (!m_PhysicsCubeList->FixedUpdate(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		if (m_TdaMiku_Model)
		{
			if (!m_TdaMiku_Model->FixedUpdate(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}

	bool CScriptScene::Dispatch(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;
		
		if (m_SimpleMorphObj)
		{
			if (!m_SimpleMorphObj->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
		}
		
		if (m_BrainStemObj)
		{
			if (!m_BrainStemObj->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
		}

		if (m_WarrokObject)
		{
			if (!m_WarrokObject->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
		}

		if (m_XBotObject)
		{
			if (!m_XBotObject->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
		}

		if (m_MouseyObject)
		{
			if (!m_MouseyObject->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
		}
		
		if (m_PhysicsGround)
		{
			if (!m_PhysicsGround->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
		}
		
		if (m_PhysicsSphere)
		{
			if (!m_PhysicsSphere->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
		}
		
		if (m_PhysicsCubeList)
		{
			if (!m_PhysicsCubeList->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
		}

		if (m_TdaMiku_Model)
		{
			if (!m_TdaMiku_Model->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, m_DebugSphere)) return false;
			//if (!m_TdaMiku_Model->Draw(IsDepthPass, true, Camera, Projection, DrawInfo, nullptr)) return false;
		}
		
		if (m_Background)
		{
			if (!m_Background->Draw(IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	// Tex of FrameBuffer
	void CScriptScene::SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
	}
}