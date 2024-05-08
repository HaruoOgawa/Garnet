#pragma once

#include <set>
#include <string>
#include <memory>

namespace resource { class CResourceManager; }

namespace app
{
	class CFileModifier
	{
		std::set<std::string> m_EditingFileNameSet;
	public:
		CFileModifier();

		virtual ~CFileModifier() = default;

		void AddEditingFileName(const std::string& FileName);

		void OnFileUpdated(const std::shared_ptr<resource::CResourceManager>& ResourceManager);
	};
}