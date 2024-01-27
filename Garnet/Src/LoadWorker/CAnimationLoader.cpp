#ifdef USE_ANIMATION
#include "CAnimationLoader.h"

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

	bool CAnimationLoader::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_File->IsLoaded()) return true;

		if (!m_Target->Import(pGraphicsAPI, m_File->GetData(), m_FileName, m_File->GetExtention())) return false;

		// ƒ[ƒhŠ®—¹
		m_Status = resource::ELoadStatus::Loaded;

		return true;
	}
}
#endif