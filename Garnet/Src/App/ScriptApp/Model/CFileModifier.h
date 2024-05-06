#pragma once

#include <set>
#include <string>

namespace app
{
	class CFileModifier
	{
		std::set<std::string> m_EditingFileNameSet;
	public:
		CFileModifier();

		virtual ~CFileModifier() = default;

		void AddEditingFileName(const std::string& FileName);
	};
}