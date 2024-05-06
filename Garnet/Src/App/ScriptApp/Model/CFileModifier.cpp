#include "CFileModifier.h"

namespace app
{
	CFileModifier::CFileModifier()
	{
	}

	void CFileModifier::AddEditingFileName(const std::string& FileName)
	{
		m_EditingFileNameSet.emplace(FileName);
	}
}