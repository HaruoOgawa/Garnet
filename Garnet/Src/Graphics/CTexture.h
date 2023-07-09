#pragma once

#include <vector>
#include "ETextureType.h"
#include "../GraphicsAPI/ERenderPassFormat.h"

namespace graphics
{
	class CTexture
	{
	protected:
		ETextureType m_TextureType;
		api::ERenderPassFormat m_RenderPassFormat;

		int m_Width;
		int m_Height;
		int m_NumOfChannels;

		const bool m_UseMipMap;
		bool  m_HasMipData; // 画像データ生成時に拡張子名から自動判断
		float m_MipCount;
	private:
		bool IsPowerOfTwo(int n);
	public:
		CTexture(bool UseMipMap);
		virtual ~CTexture();

		ETextureType GetTextureType() const;
		api::ERenderPassFormat GetRenderPassFormat()const;
		float GetMipCount() const;

		virtual bool CreateFrameTexture(int Width, int Height, api::ERenderPassFormat RenderPassFormat) = 0;
#ifdef USE_TEXTURE_LOADER
		virtual bool Create(const std::vector<unsigned char>& Data);
		virtual bool Create(const std::vector<std::vector<unsigned char>>& DataList);
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) = 0;
#endif // USE_TEXTURE_LOADER
	};
}