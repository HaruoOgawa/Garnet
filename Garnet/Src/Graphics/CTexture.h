#pragma once

#include <string>
#include <vector>
#include "ETextureType.h"
#include "../GraphicsAPI/ERenderPassFormat.h"
#include "STextureSamplerParam.h"

namespace graphics
{
	class CTexture
	{
	protected:
		ETextureType m_TextureType;
		api::ERenderPassFormat m_RenderPassFormat;

		std::string m_FileName;
		std::vector<std::string> m_FileNameList;

		int m_Width;
		int m_Height;
		int m_NumOfChannels;

		const bool m_UseMipMap;
		bool  m_HasMipData; // 画像データ生成時に拡張子名から自動判断
		float m_MipCount;

		const STextureSamplerParam m_SamplerParam;
	private:
		bool IsPowerOfTwo(int n);
	public:
		CTexture(bool UseMipMap, const STextureSamplerParam& SamplerParam);
		virtual ~CTexture();

		void SetFileName(const std::string& Name);
		const std::string& GetFileName() const;

		void SetFileNameList(const std::vector<std::string>& NameList);
		const std::vector<std::string>& GetFileNameList() const;

		int GetWidth() const;
		int GetHeight() const;

		ETextureType GetTextureType() const;
		void SetTextureType(ETextureType TextureType);
		api::ERenderPassFormat GetRenderPassFormat()const;
		float GetMipCount() const;

		virtual bool CreateFrameTexture(int Width, int Height, api::ERenderPassFormat RenderPassFormat) = 0;
#ifdef USE_TEXTURE_LOADER
		virtual bool Create(const std::vector<unsigned char>& Data);
		virtual bool Create(const std::vector<std::vector<unsigned char>>& DataList);
#endif // USE_TEXTURE_LOADER
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) = 0;
	};
}