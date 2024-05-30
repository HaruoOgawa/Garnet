#include "CSceneController.h"
#include "../Object/C3DObject.h"

namespace scene
{
	CSceneController::CSceneController()
	{
	}

	CSceneController::~CSceneController()
	{
	}

	void CSceneController::AddObject(const std::shared_ptr<object::C3DObject>& Object)
	{
		m_ObjectList.push_back(Object);
	}

	std::vector<std::shared_ptr<object::C3DObject>> CSceneController::GetObjectList() const
	{
		return m_ObjectList;
	}

	void CSceneController::AddMaterialFrame(const std::string& MFName, const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		m_MaterialFrameMap.emplace(MFName, MaterialFrame);
	}

	bool CSceneController::Create(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine)
	{
		for (const auto& Object : m_ObjectList)
		{
			// ‰¼ŽÀ‘•
			const auto& MaterialFrame = m_MaterialFrameMap.find("PBR_MF");
			if (MaterialFrame == m_MaterialFrameMap.end()) return false;
			auto Material = MaterialFrame->second->CreateMaterial(pGraphicsAPI, static_cast<int>(Object->GetNodeList().size()), graphics::ECullMode::CULL_BACK);

			Object->AddMaterial(Material);

			if (!Object->Create(pGraphicsAPI, pPhysicsEngine, nullptr)) return false;
		}

		return true;
	}

	bool CSceneController::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		for (const auto& Object : m_ObjectList)
		{
			if (!Object->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}
	bool CSceneController::LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		for (const auto& Object : m_ObjectList)
		{
			if (!Object->LateUpdate(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}

	bool CSceneController::FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		for (const auto& Object : m_ObjectList)
		{
			if (!Object->FixedUpdate(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}

	bool CSceneController::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		for (const auto& Object : m_ObjectList)
		{
			if (!Object->Draw(IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}
}