#include "CSceneLoader.h"

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

	bool CSceneLoader::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, const std::shared_ptr<CResourceManager>& ResourceManager)
	{
		if (!m_File->IsLoaded()) return true;

		// ÉçÅ[ÉhäÆóπ
		m_Status = resource::ELoadStatus::Loaded;

		return true;
	}
}