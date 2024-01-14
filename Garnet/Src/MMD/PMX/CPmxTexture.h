#pragma once

#ifdef USE_MMD
#include <string>

namespace mmd
{
	class CPmxTexture
	{
		std::pair<std::string, std::wstring> m_FilePath;
	public:
		CPmxTexture();

		const std::pair<std::string, std::wstring>& GetFilePath() const;
		void SetFilePath(const std::pair<std::string, std::wstring>& FilePath);
	};
}
#endif