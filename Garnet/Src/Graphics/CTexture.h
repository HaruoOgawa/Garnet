#pragma once

#include <vector>

namespace graphics
{
	enum class ETextureType
	{
		TEXTURE_2D,
		TEXTURE_CUBE,
	};

	class CTexture
	{
	protected:
		ETextureType m_TextureType;

		int m_Width;
		int m_Height;
		int m_NumOfChannels;
	public:
		CTexture();
		virtual ~CTexture();

		ETextureType GetTextureType() const;

#ifdef USE_TEXTURE_LOADER
		virtual bool Create(const std::vector<unsigned char>& Data);
		virtual bool Create(const std::vector<std::vector<unsigned char>>& DataList);
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) = 0;
#endif // USE_TEXTURE_LOADER
	};
}