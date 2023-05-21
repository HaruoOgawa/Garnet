#pragma once

#include <vector>

namespace graphics
{
	class CTexture
	{
	protected:
		int m_Width;
		int m_Height;
		int m_NumOfChannels;
	public:
		CTexture();
		virtual ~CTexture();

#ifdef USE_TEXTURE_LOADER
		virtual bool Create(const std::vector<char>& Data);
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) = 0;
#endif // USE_TEXTURE_LOADER
	};
}