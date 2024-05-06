#pragma once

#include "../../Interface/IResource.h"
#include <string>
#include <map>
#include <memory>
#include <filesystem>

namespace resource
{
	struct SMemoryResource
	{
		std::string FileName = "";
		std::shared_ptr<IResource> ResourceData = nullptr;
		std::filesystem::file_time_type FinalEditTime = std::filesystem::file_time_type();
		// リソースを参照しているリソース
		std::vector<std::shared_ptr<IResource>> ParentResourceDataList;
	};

	class CResourceManager
	{
		std::map<std::string, SMemoryResource> m_OnMemoryResourceList;
	public:
		CResourceManager();
		virtual ~CResourceManager();

		void AddOnMemoryResource(const std::shared_ptr<IResource>& Resource, const std::shared_ptr<IResource>& ParentResource);
	};
}


