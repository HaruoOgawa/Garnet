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
		auto it = m_OnMemoryResourceList.find(Resource->GetFilename());

		if (it == m_OnMemoryResourceList.end())
		{
			// 新規作成
			SMemoryResource MemoryResource = {};
			MemoryResource.FileName = Resource->GetFilename();
			MemoryResource.ResourceData = Resource;
#ifndef __EMSCRIPTEN__
			// Emscriptenはサポートしない
			MemoryResource.FinalEditTime = std::filesystem::last_write_time(Resource->GetFilename());
#endif // !__EMSCRIPTEN__
			if(ParentResource) MemoryResource.ParentResourceDataList.push_back(ParentResource);

			m_OnMemoryResourceList.emplace(MemoryResource.FileName, MemoryResource);
		}
		else
		{
			if (ParentResource) it->second.ParentResourceDataList.push_back(ParentResource);
		}
	}
}