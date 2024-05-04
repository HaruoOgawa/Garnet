#pragma once

#include "../../Interface/IResource.h"
#include <string>
#include <map>
#include <memory>

namespace resource
{
	class CResourceManager
	{
		std::map<std::string, std::shared_ptr<IResource>> m_OnMemoryResourceList;

		std::map<std::string, std::shared_ptr<IResource>> m_EditingResourceList;
	public:
		CResourceManager();
		virtual ~CResourceManager();

		void AddOnMemoryResource(const std::string& Path, const std::shared_ptr<IResource>& Resouce);

		void AddEditingResource(const std::string& Path);

		bool RecreateIfEdited();
	};
}


