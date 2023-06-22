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

		const bool m_UseMipMap;  // コンストラクタでのみ設定可能
		bool  m_HasMipData; // 画像データ生成時に拡張子名から自動判断
		float m_MipCount;
	private:
		bool IsPowerOfTwo(int n);
	public:
		CTexture(bool UseMipMap);
		virtual ~CTexture();

		ETextureType GetTextureType() const;
		float GetMipCount() const;
#ifdef USE_TEXTURE_LOADER
		virtual bool Create(const std::vector<unsigned char>& Data);
		virtual bool Create(const std::vector<std::vector<unsigned char>>& DataList);
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) = 0;
#endif // USE_TEXTURE_LOADER
	};
}