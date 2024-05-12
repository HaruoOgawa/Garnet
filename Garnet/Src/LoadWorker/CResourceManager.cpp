#include "CResourceManager.h"

namespace resource
{
	CResourceManager::CResourceManager()
	{
	}

	CResourceManager::~CResourceManager()
	{
	}

	void CResourceManager::AddOnMemoryResource(const std::shared_ptr<IResource>& Resource, const std::shared_ptr<IResource>& ParentResource)
	{
		std::string Filename = ExchangeYenToSlash(Resource->GetFilename());

		auto it = m_OnMemoryResourceList.find(Filename);

		if (it == m_OnMemoryResourceList.end())
		{
			// 新規作成
			SMemoryResource MemoryResource = {};
			MemoryResource.FileName = Filename;
			MemoryResource.ResourceData = Resource;
#ifndef __EMSCRIPTEN__
			// Emscriptenはサポートしない
			MemoryResource.FinalEditTime = std::filesystem::last_write_time(Filename);
#endif // !__EMSCRIPTEN__
			if(ParentResource) MemoryResource.ParentResourceDataList.push_back(ParentResource);

			m_OnMemoryResourceList.emplace(MemoryResource.FileName, MemoryResource);
		}
		else
		{
			if (ParentResource) it->second.ParentResourceDataList.push_back(ParentResource);
		}
	}

	const std::map<std::string, SMemoryResource>& CResourceManager::GetOnMemoryResourceList() const
	{
		return m_OnMemoryResourceList;
	}

	std::shared_ptr<IResource> CResourceManager::FindResource(const std::string& FileName)
	{
		const auto& it = m_OnMemoryResourceList.find(FileName);
		if (it == m_OnMemoryResourceList.end()) return nullptr;

		return it->second.ResourceData;
	}

	void CResourceManager::UpdateFinalEditTime(const std::string& FileName, std::filesystem::file_time_type FinalEditTime)
	{
		if (m_OnMemoryResourceList.find(FileName) == m_OnMemoryResourceList.end()) return;

		m_OnMemoryResourceList[FileName].FinalEditTime = FinalEditTime;
	}

	std::string CResourceManager::ExchangeYenToSlash(const std::string& SrcName)
	{
		std::string DstName = SrcName;

		while (DstName.find("\\") != -1)
		{
			DstName.replace(DstName.find("\\"), 1, "/");
		}
		return DstName;
	}
}