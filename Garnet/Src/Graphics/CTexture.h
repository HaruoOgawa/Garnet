#pragma once

#include <vector>

namespace graphics
{
	class CTexture
	{
		int m_Width;
		int m_Height;
		int m_NumOfChannels;
	public:
		CTexture();
		virtual ~CTexture();

		virtual bool Create(const std::vector<char>& Data);
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize);
	};
}