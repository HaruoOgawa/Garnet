#include "CTexture.h"

#if defined(USE_TEXTURE_LOADER) && !defined(__EMSCRIPTEN__) 
#define STB_IMAGE_IMPLEMENTATION
#include "../Library/stb/stb_image.h"
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

	bool CTexture::Create(const std::vector<char>& Data)
	{
#if defined(USE_TEXTURE_LOADER) && !defined(__EMSCRIPTEN__)
		// stbiでテクスチャバイナリを解析してピクセルデータを取得する
		stbi_uc* stbi_pixelData = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(&Data[0]), Data.size(), &m_Width, &m_Height, &m_NumOfChannels, STBI_rgb_alpha);

		// stbiから取得したピクセルデータを扱いやすいデータにコピーしておく
		int pixelSize = m_Width * m_Height * 4;
		std::vector<unsigned char> pixelData(pixelSize);
		std::memcpy(&pixelData[0], stbi_pixelData, pixelSize);

		// stbiのメモリを解放
		stbi_image_free(stbi_pixelData);

		// APIにデータを渡す
		if (!Create(pixelData, pixelSize)) return false;
#elif defined(__EMSCRIPTEN__)
		
#endif
		return true;
	}

	bool CTexture::Create(const std::vector<unsigned char>& pixelData, int pixelSize)
	{
		return true;
	}
}