#include "CTexture.h"

#if defined(USE_TEXTURE_LOADER)
#define STB_IMAGE_STATIC
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // !STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif // USE_TEXTURE_LOADER

namespace graphics
{
	CTexture::CTexture():
		m_Width(0),
		m_Height(0),
		m_NumOfChannels(0)
	{
	}

	CTexture::~CTexture()
	{
	}

#ifdef USE_TEXTURE_LOADER
	bool CTexture::Create(const std::vector<unsigned char>& Data)
	{
		// stbiでテクスチャバイナリを解析してピクセルデータを取得する
		stbi_uc* stbi_pixelData = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(&Data[0]), static_cast<int>(Data.size()), &m_Width, &m_Height, &m_NumOfChannels, STBI_rgb_alpha);

		// stbiから取得したピクセルデータを扱いやすいデータにコピーしておく
		int pixelSize = m_Width * m_Height * 4;
		std::vector<unsigned char> pixelData(pixelSize);
		std::memcpy(&pixelData[0], stbi_pixelData, pixelSize);

		// stbiのメモリを解放
		stbi_image_free(stbi_pixelData);

		// APIにデータを渡す
		if (!Create(pixelData, pixelSize)) return false;
		
		return true;
	}

	bool CTexture::Create(const std::vector<unsigned char>& pixelData, int pixelSize)
	{
		return true;
	}
#endif // USE_TEXTURE_LOADER
}