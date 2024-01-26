#ifdef USE_MMD
#include "CPmxTexture.h"

namespace mmd
{
	CPmxTexture::CPmxTexture():
		m_FilePath(std::make_pair(std::string(""), std::wstring(L"")))
	{
	}

	const std::pair<std::string, std::wstring>& CPmxTexture::GetFilePath() const
	{
		return m_FilePath;
	}

	void CPmxTexture::SetFilePath(const std::pair<std::string, std::wstring>& FilePath)
	{
		m_FilePath = FilePath;
	}
}
#endif