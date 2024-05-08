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

	private:
		std::string ExchangeYenToSlash(const std::string& SrcName);
	public:
		CResourceManager();
		virtual ~CResourceManager();

		void AddOnMemoryResource(const std::shared_ptr<IResource>& Resource, const std::shared_ptr<IResource>& ParentResource);

		const std::map<std::string, SMemoryResource>& GetOnMemoryResourceList() const;

		void UpdateFinalEditTime(const std::string& FileName, std::filesystem::file_time_type FinalEditTime);
	};
}


