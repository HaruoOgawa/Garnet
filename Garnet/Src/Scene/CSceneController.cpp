#include "CSceneController.h"
#include "../Object/C3DObject.h"
#include "../Message/Console.h"

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

	void CSceneController::AddMaterialInfo(const std::shared_ptr<object::C3DObject>& Object, const std::vector<SMaterialInfo>& MaterialInfoList)
	{
		m_MaterialInfoMap.emplace(Object, MaterialInfoList);
	}

	bool CSceneController::Create(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine)
	{
		for (const auto& Object : m_ObjectList)
		{
			if (Object->GetMaterialList().size() == 0)
			{
				// MaterialList‚ð¶¬
				if (!CreateMaterialList(pGraphicsAPI, Object)) return false;
			}
			else
			{
				// Materialî•ñ‚ðXV
				if (!UpdateMaterialUniform(pGraphicsAPI, Object)) return false;
			}

			// Object¶¬
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

	bool CSceneController::CreateMaterialList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object)
	{
		// MaterialInfo‚ðŽæ“¾
		const auto& it = m_MaterialInfoMap.find(Object);
		if (it == m_MaterialInfoMap.end())
		{
			Console::Log("[SceneController Error] MaterialInfo Not Found\n");

			return false;
		}

		const auto& MaterialInfoList = it->second;

		for (const auto& MaterialInfo : MaterialInfoList)
		{
			// MaterialFrame‚ðŽæ“¾
			const auto& MaterialFrame = m_MaterialFrameMap.find(MaterialInfo.MaterialFrameName);
			if (MaterialFrame == m_MaterialFrameMap.end())
			{
				Console::Log("[SceneController Error] MaterialFrame Not Found\n");

				return false;
			}

			// Material‚ð¶¬
			auto Material = MaterialFrame->second->CreateMaterial(pGraphicsAPI, MaterialInfo.RefCount, MaterialInfo.CullMode);

			// UniformValue‚ðÝ’è
			for (const auto& UniformInfo : MaterialInfo.UniformInfoList)
			{
				Material->SetUniformValue(UniformInfo.UniformName, &UniformInfo.UniformData[0], UniformInfo.ByteSize);
			}

			// Material‚ð“o˜^
			Object->AddMaterial(Material);
		}

		return true;
	}

	bool CSceneController::UpdateMaterialUniform(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object)
	{
		// MaterialInfo‚ðŽæ“¾
		const auto& it = m_MaterialInfoMap.find(Object);
		if (it == m_MaterialInfoMap.end()) return true;

		//
		const auto& MaterialInfoList = it->second;
		const auto& MaterialList = Object->GetMaterialList();

		if (MaterialInfoList.size() != MaterialList.size()) return true;

		//
		for (int MaterialIndex = 0; MaterialIndex < MaterialList.size(); MaterialIndex++)
		{
			const auto& MaterialInfo = MaterialInfoList[MaterialIndex];
			const auto& Material = MaterialList[MaterialIndex];

			// UniformValue‚ðÝ’è
			for (const auto& UniformInfo : MaterialInfo.UniformInfoList)
			{
				Material->SetUniformValue(UniformInfo.UniformName, &UniformInfo.UniformData[0], UniformInfo.ByteSize);
			}
		}

		return true;
	}
}