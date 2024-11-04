#ifdef USE_ANIMATION
#include "CAnimationLoader.h"
#include "CLoadWorker.h"

namespace resource
{
	CAnimationLoader::CAnimationLoader(const std::string& FileName, const std::shared_ptr<animation::CAnimationClipSet>& Target, animation::ERigType RigType) :
		CResource(FileName),
		m_Target(Target),
		m_TargetRigType(RigType)
	{
		m_Target->SetFileName(FileName);
	}

	CAnimationLoader::~CAnimationLoader()
	{
	}

	bool CAnimationLoader::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::CApp* pApp)
	{
		if (!m_File->IsLoaded())
		{
			if (!m_File->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, pApp)) return false;
			return true;
		}

		if (!m_Target->Import(pGraphicsAPI, m_File->GetData(), m_FileName, m_File->GetExtention(), m_TargetRigType)) return false;

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		// リソースマネージャーに登録
		pLoadWorker->GetResourceManager()->AddOnMemoryResource(shared_from_this(), nullptr);

		return true;
	}
}
#endif