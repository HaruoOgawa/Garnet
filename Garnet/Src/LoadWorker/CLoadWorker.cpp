#include "CLoadWorker.h"

namespace resource
{
	CLoadWorker::CLoadWorker()
	{
	}

	CLoadWorker::~CLoadWorker()
	{
		m_LoadResourceList.clear();
	}

	bool CLoadWorker::Update()
	{
		if (m_LoadResourceList.size() <= 0) return true;

		for (auto& Resource : m_LoadResourceList)
		{
			if (Resource->IsLoaded()) continue;

			if (Resource->GetStatus() == resource::ELoadStatus::Loading) return true;

			if (!Resource->Load()) return false;

			return true;
		}

		m_LoadResourceList.clear();

		return true;
	}

	bool CLoadWorker::Draw()
	{
		return true;
	}

	bool CLoadWorker::IsLoaded()
	{
		return (m_LoadResourceList.size() <= 0);
	}

	void CLoadWorker::AddLoadResource(const std::shared_ptr<resource::IResource>& Resource)
	{
		m_LoadResourceList.push_back(Resource);
	}
}