#ifdef USE_ANIMATION
#include "CAnimationLoader.h"
#include "CResourceManager.h"

namespace resource
{
	CAnimationLoader::CAnimationLoader(const std::string& FileName, const std::shared_ptr<animation::CAnimationClipSet>& Target) :
		m_Status(ELoadStatus::None),
		m_File(std::make_shared<CFile>(FileName)),
		m_FileName(FileName),
		m_Target(Target)
	{
	}

	CAnimationLoader::~CAnimationLoader()
	{
	}

	const std::string& CAnimationLoader::GetFilename() const
	{
		return m_File->GetFilename();
	}

	void CAnimationLoader::SetLoadStatus(resource::ELoadStatus Status)
	{
		m_Status = Status;
	}

	resource::ELoadStatus CAnimationLoader::GetStatus() const
	{
		return m_Status;
	}

	bool CAnimationLoader::IsLoaded() const
	{
		return (m_Status == resource::ELoadStatus::Loaded);
	}

	bool CAnimationLoader::Load()
	{
		m_Status = resource::ELoadStatus::Loading;

		if (!m_File->Load()) return false;

		return true;
	}

	bool CAnimationLoader::LoadImmediate()
	{
		return true;
	}

	bool CAnimationLoader::Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager)
	{
		if (!m_File->IsLoaded())
		{
			if (!m_File->Update(pGraphicsAPI, ResourceManager)) return false;
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