#pragma once

#include <set>
#include <string>
#include <memory>

namespace resource { 
	class CResourceManager; 
	class IResource;
}

namespace app
{
	class CFileModifier
	{
		std::set<std::string> m_EditingFileNameSet;

		std::set<std::shared_ptr<resource::IResource>> m_UpdateReservedResourceSet;
	public:
		CFileModifier();

		virtual ~CFileModifier() = default;

		void AddEditingFileName(const std::string& FileName);

		void OnFileUpdated(const std::shared_ptr<resource::CResourceManager>& ResourceManager);
	};
}