#ifdef USE_ANIMATION
#include "CAnimationLoader.h"
#include "CResourceManager.h"

namespace resource
{
	CAnimationLoader::CAnimationLoader(const std::string& FileName, const std::shared_ptr<animation::CAnimationClipSet>& Target) :
		CResource(FileName),
		m_Target(Target)
	{
	}

	CAnimationLoader::~CAnimationLoader()
	{
	}

	bool CAnimationLoader::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, const std::shared_ptr<CResourceManager>& ResourceManager)
	{
		if (!m_File->IsLoaded())
		{
			if (!m_File->Update(pGraphicsAPI, pPhysicsEngine, ResourceManager)) return false;
			return true;
		}

		if (!m_Target->Import(pGraphicsAPI, m_File->GetData(), m_FileName, m_File->GetExtention())) return false;

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		// リソースマネージャーに登録
		ResourceManager->AddOnMemoryResource(shared_from_this(), nullptr);

		return true;
	}
}
#endif