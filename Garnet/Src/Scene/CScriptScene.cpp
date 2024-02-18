#include "CScriptScene.h"

#include "../LoadWorker/CLoadWorker.h"
#include "../LoadWorker/CFile.h"
#include "../LoadWorker/CMaterialFrameLoader.h"
#include "../LoadWorker/CTextureLoader.h"
#include "../LoadWorker/C3DObjectLoader.h"
#include "../LoadWorker/CAnimationLoader.h"

#include "../Debug/Message/Console.h"

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

		m_PhysicsGround(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_PhysicsSphere(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_PhysicsCubeList(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_TdaMiku_Model(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_VMDAnimationSet(std::make_shared<animation::CAnimationClipSet>()),

		m_Background(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_IBL_Skybox_Texture(pGraphicsAPI->CreateTexture(false)),
		m_Cube_Texture(pGraphicsAPI->CreateTexture(false)),

		m_IsLoaded(false)
	{
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\basic_toon_mf.json", m_BasicToonMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\SimpleTexture_MF.json", m_SimpleTextureMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\Depth_MF.json", m_DepthMF));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\PBR_MF.json", m_PBRMF));
		
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::C3DObjectLoader>("Resources\\Avatar\\Tda_Miku\\Tda_Miku.pmx", m_TdaMiku_Model, "", "ShadowPass"));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\mmd_running.vmd", m_VMDAnimationSet));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\Run_m4th_Loop.vmd", m_VMDAnimationSet));
		//pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CAnimationLoader>("Resources\\Motions\\BackFlip.vmd", m_VMDAnimationSet));
		
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\IBL\\output_skybox.hdr" }), m_IBL_Skybox_Texture));
		pLoadWorker->AddFirstLoadResource(std::make_shared<resource::CTextureLoader>(pGraphicsAPI, std::vector<std::string>({ "Resources\\Cubemaps\\environment\\environment_back_0.jpg", "Resources\\Cubemaps\\environment\\environment_bottom_0.jpg", "Resources\\Cubemaps\\environment\\environment_front_0.jpg", "Resources\\Cubemaps\\environment\\environment_left_0.jpg", "Resources\\Cubemaps\\environment\\environment_right_0.jpg", "Resources\\Cubemaps\\environment\\environment_top_0.jpg" }), m_Cube_Texture));
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
		// m_PhysicsGround
		{
			auto Material = m_PBRMF->CreateMaterial(pGraphicsAPI, 1, graphics::ECullMode::CULL_BACK);

			m_PhysicsGround->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			Material->ReplacePreloadUniformValue("useCubeMap", &glm::ivec1(1)[0], sizeof(glm::ivec1), 0);
			Material->ReplaceTextureIndex("cubemapTexture", 0);
			Material->ReplacePreloadUniformValue("roughnessFactor", &glm::vec1(1.0f)[0], sizeof(float), 0);

			std::shared_ptr<math::CTransform> LocalTransform = std::make_shared<math::CTransform>();
			LocalTransform->SetPos(glm::vec3(0.0f, -1.0f, 0.0f));
			LocalTransform->SetScale(glm::vec3(5.0f, 0.1f, 5.0f));

			auto PhysicsBox = pPhysicsEngine->CreatePhysicsBox(glm::vec3(0.5f), true, 0.0f);

			if (!m_PhysicsGround->CreateSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateBox(), Material, m_DepthMF, LocalTransform, PhysicsBox)) return false;
		}

		// m_PhysicsSphere
		{
			// Material
			auto Material = m_PBRMF->CreateMaterial(pGraphicsAPI, 3, graphics::ECullMode::CULL_BACK);

			m_PhysicsSphere->GetTextureSet()->AddCubeMap(m_Cube_Texture);
			Material->ReplacePreloadUniformValue("useCubeMap", &glm::ivec1(1)[0], sizeof(glm::ivec1), 0);
			Material->ReplaceTextureIndex("cubemapTexture", 0);
			Material->ReplacePreloadUniformValue("roughnessFactor", &glm::vec1(1.0f)[0], sizeof(float), 0);

			m_PhysicsSphere->AddMaterial(Material);

			// Mesh
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
			Mesh->CreateSimpleMesh(graphics::CPresetPrimitive::CreateSphere(), 0);

			m_PhysicsSphere->AddMesh(Mesh);

			// Node
			{
				std::shared_ptr<math::CTransform> LocalTransform = std::make_shared<math::CTransform>();
				LocalTransform->SetPos(glm::vec3(0.0f, 1.5f, 0.0f));
				LocalTransform->SetScale(glm::vec3(0.1f));

				auto PhysicsSphere = pPhysicsEngine->CreatePhysicsSphere(1.0f, true, 0.0f);

				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, -1);
				Node->SetLocalTransform(LocalTransform);
				Node->SetPhysicsObject(PhysicsSphere);

				m_PhysicsSphere->AddNode(Node);
			}

			{
				std::shared_ptr<math::CTransform> LocalTransform = std::make_shared<math::CTransform>();
				LocalTransform->SetPos(glm::vec3(1.0f, 1.5f, 0.0f));
				LocalTransform->SetScale(glm::vec3(0.1f));

				auto PhysicsSphere = pPhysicsEngine->CreatePhysicsSphere(1.0f, true, 0.0f);

				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, -1);
				Node->SetLocalTransform(LocalTransform);
				Node->SetPhysicsObject(PhysicsSphere);

				m_PhysicsSphere->AddNode(Node);
			}

			{
				std::shared_ptr<math::CTransform> LocalTransform = std::make_shared<math::CTransform>();
				LocalTransform->SetPos(glm::vec3(2.0f, 1.5f, 0.0f));
				LocalTransform->SetScale(glm::vec3(0.1f));

				auto PhysicsSphere = pPhysicsEngine->CreatePhysicsSphere(1.0f, true, 0.0f);

				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, -1);
				Node->SetLocalTransform(LocalTransform);
				Node->SetPhysicsObject(PhysicsSphere);

				m_PhysicsSphere->AddNode(Node);
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
			Mesh->CreateSimpleMesh(graphics::CPresetPrimitive::CreateBox(), 0);

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
						LocalTransform->SetPos(glm::vec3(xpos * size * 0.5f, ypos * size * 0.5f, zpos * size * 0.5f));
						LocalTransform->SetScale(glm::vec3(size));

						auto PhysicsBox = pPhysicsEngine->CreatePhysicsBox(glm::vec3(0.5f), false, 1.0f);

						std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, -1);
						Node->SetLocalTransform(LocalTransform);
						Node->SetPhysicsObject(PhysicsBox);

						m_PhysicsCubeList->AddNode(Node);
					}
				}
			}

			// Create
			if (!m_PhysicsCubeList->Create(pGraphicsAPI, pPhysicsEngine, m_DepthMF)) return false;
		}

		// m_TdaMiku_Model
		/*{
			if (!m_TdaMiku_Model->CreateFromMemory(pGraphicsAPI, pPhysicsEngine, pLoadWorker, m_BasicToonMF, m_DepthMF, object::E3DObjectType::Pmx)) return false;

			m_TdaMiku_Model->SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
			m_TdaMiku_Model->SetScale(glm::vec3(0.1f));

			auto Clip = m_VMDAnimationSet->GetAnimationClip(0);
			if (Clip) m_TdaMiku_Model->AddHumanoidAnimationClip(Clip, "Walk", { nullptr, "" }, true);

			m_TdaMiku_Model->ChangeMotion("Walk");
		}*/

		// m_Background
		{
			auto Mat = m_SimpleTextureMF->CreateMaterial(pGraphicsAPI, 1, graphics::ECullMode::CULL_FRONT);
			Mat->ReplacePreloadUniformValue("useDirSampling", &glm::ivec1(1)[0], sizeof(glm::ivec1), 1);
			Mat->ReplacePreloadUniformValue("useTexColor", &glm::ivec1(1)[0], sizeof(glm::ivec1), 1);
			Mat->ReplaceTextureIndex("texImage", 0);

			m_Background->GetTextureSet()->Add2DTexture(m_IBL_Skybox_Texture);

			m_Background->SetScale(glm::vec3(500.0f));
			if (!m_Background->CreateSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateSphere(), Mat , m_DepthMF)) return false;
		}

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

		if (m_PhysicsGround)
		{
			if (!m_PhysicsGround->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_PhysicsSphere)
		{
			if (!m_PhysicsSphere->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}
		
		if (m_PhysicsCubeList)
		{
			if (!m_PhysicsCubeList->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		/*if (m_TdaMiku_Model)
		{
			if (!m_TdaMiku_Model->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}*/
		
		if (m_Background)
		{
			if (!m_Background->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}

	bool CScriptScene::Dispatch(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;
		
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

		/*if (m_TdaMiku_Model)
		{
			if (!m_TdaMiku_Model->Draw(IsDepthPass, false, Camera, Projection, DrawInfo, nullptr)) return false;
			//if (!m_TdaMiku_Model->Draw(IsDepthPass, true, Camera, Projection, DrawInfo, nullptr)) return false;
		}*/
		
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