#include "CResource.h"

namespace resource
{
	CResource::CResource(const std::string& FileName):
		m_Status(ELoadStatus::None),
		m_File(std::make_shared<CFile>(FileName)),
		m_FileName(FileName)
	{
	}

	CResource::~CResource()
	{
	}

	bool CResource::Load()
	{
		m_Status = resource::ELoadStatus::Loading;

		if (!m_File->Load()) return false;

		return true;
	}

	bool CResource::LoadImmediate()
	{
		return true;
	}

	const std::string& CResource::GetFilename() const
	{
		return m_FileName;
	}

	void CResource::SetLoadStatus(resource::ELoadStatus Status)
	{
		m_Status = Status;
	}

	resource::ELoadStatus CResource::GetStatus() const
	{
		return m_Status;
	}

	bool CResource::IsLoaded() const
	{
		return (m_Status == resource::ELoadStatus::Loaded);
	}
}