#include "CTexture.h"

namespace graphics
{
	CTexture::CTexture():
		m_Width(0),
		m_Height(0)
	{
	}

	CTexture::~CTexture()
	{
	}

	bool CTexture::Create(const std::vector<char>& Data)
	{
		return true;
	}
}