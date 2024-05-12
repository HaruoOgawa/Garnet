#pragma once

#include <set>
#include <map>
#include <string>
#include <memory>

namespace resource {
	class CLoadWorker;
	class CResourceManager; 
	class IResource;
}

namespace app
{
	class CFileModifier
	{
		std::set<std::string> m_EditingFileNameSet;
	public:
		CFileModifier();

		virtual ~CFileModifier() = default;

		void AddEditingFileName(const std::string& FileName);

		void OnFileUpdated(resource::CLoadWorker* pLoadWorker);
	};
}