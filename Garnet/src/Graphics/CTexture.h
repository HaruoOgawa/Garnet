#pragma once

#include <vector>

namespace graphics
{
	class CTexture
	{
		unsigned int m_Width;
		unsigned int m_Height;
	public:
		CTexture();
		virtual ~CTexture();

		virtual bool Create(const std::vector<char>& Data);
	};
}