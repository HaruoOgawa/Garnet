#include "CTexture.h"

#include "../Message/Console.h"

#if defined(USE_TEXTURE_LOADER)
	#ifdef USE_PNG_PARSER
		#include "../Image/CPNGParserer.h"
	#else
		#define STB_IMAGE_STATIC
		#ifndef STB_IMAGE_IMPLEMENTATION
		#define STB_IMAGE_IMPLEMENTATION
		#endif // !STB_IMAGE_IMPLEMENTATION
		#include <stb_image.h>
	#endif // USE_PNG_PARSER
#endif // USE_TEXTURE_LOADER

namespace graphics
{
	CTexture::CTexture(bool UseMipMap, const STextureSamplerParam& SamplerParam):
		m_TextureType(ETextureType::TEXTURE_2D),
		m_RenderPassFormat(api::ERenderPassFormat::NONE),
		m_FileName(std::string()),
		m_Width(1),
		m_Height(1),
		m_NumOfChannels(1),
		m_UseMipMap(UseMipMap),
		m_HasMipData(false),
		m_MipCount(1.0f),
		m_SamplerParam(SamplerParam)
	{
	}

	CTexture::~CTexture()
	{
	}

	bool CTexture::IsUseMipMap() const
	{
		return m_UseMipMap;
	}

	const STextureSamplerParam& CTexture::GetSamplerParam() const
	{
		return m_SamplerParam;
	}

	void CTexture::SetFileName(const std::string& Name)
	{
		m_FileName = Name;
	}

	const std::string& CTexture::GetFileName() const
	{
		return m_FileName;
	}

	void CTexture::SetFileNameList(const std::vector<std::string>& NameList)
	{
		m_FileNameList = NameList;
	}

	const std::vector<std::string>& CTexture::GetFileNameList() const
	{
		return m_FileNameList;
	}

	int CTexture::GetWidth() const
	{
		return m_Width;
	}

	int CTexture::GetHeight() const
	{
		return m_Height;
	}

	ETextureType CTexture::GetTextureType() const
	{
		return m_TextureType;
	}

	void CTexture::SetTextureType(ETextureType TextureType)
	{
		m_TextureType = TextureType;
	}

	api::ERenderPassFormat CTexture::GetRenderPassFormat()const
	{
		return m_RenderPassFormat;
	}

	float CTexture::GetMipCount() const
	{
		return m_MipCount;
	}

	bool CTexture::CreateFrameTexture(int Width, int Height, api::ERenderPassFormat RenderPassFormat, int AASampleNum)
	{
		return true;
	}

#ifdef USE_TEXTURE_LOADER
	bool CTexture::Create(const std::vector<unsigned char>& Data)
	{
		// 単一のピクセル配列を使用しているので2D
		m_TextureType = ETextureType::TEXTURE_2D;

#ifdef USE_PNG_PARSER
		// png parser ////////////////////////////////////////////////////////////////////////////////////////////////////
		std::vector<unsigned char> pixelData;
		if (!image_parse::CPNGParserer::Parse(Data, pixelData, m_Width, m_Height))
		{
			Console::Log("[Error] Failed to parse PNG Image.\n");
			return false;
		}

		int pixelSize = static_cast<int>(pixelData.size());
#else
		// stbi /////////////////////////////////////////////////////////////////////////////////////////////////////////
		// stbiでテクスチャバイナリを解析してピクセルデータを取得する
		stbi_uc* stbi_pixelData = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(&Data[0]), static_cast<int>(Data.size()), &m_Width, &m_Height, &m_NumOfChannels, STBI_rgb_alpha);

		// stbiから取得したピクセルデータを扱いやすいデータにコピーしておく
		int pixelSize = m_Width * m_Height * 4;
		std::vector<unsigned char> pixelData(pixelSize);
		std::memcpy(&pixelData[0], stbi_pixelData, pixelSize);

		// stbiのメモリを解放
		stbi_image_free(stbi_pixelData);
#endif // USE_PNG_PARSER

		// MipCountを計算
		int CurrentSize = (m_Width < m_Height) ? m_Width : m_Height;
		int MipCount = 0;

		if (IsPowerOfTwo(CurrentSize))
		{
			while (CurrentSize != 1)
			{
				CurrentSize /= 2;
				MipCount++;
			}

			m_MipCount = static_cast<float>(MipCount);
		}

		// APIにデータを渡す
		if (!Create(pixelData, pixelSize)) return false;
		
		return true;
	}

	bool CTexture::Create(const std::vector<std::vector<unsigned char>>& DataList)
	{
		if (DataList.size() < 6) return false;

		// 複数のピクセル配列を使用しているのでCUBE
		m_TextureType = ETextureType::TEXTURE_CUBE;

		// データを取得
		std::vector<std::vector<unsigned char>> pixelDataList;
		std::vector<int> pixelSizeList;

#ifdef USE_PNG_PARSER
		// png parser ////////////////////////////////////////////////////////////////////////////////////////////////////

#else
		// stbi /////////////////////////////////////////////////////////////////////////////////////////////////////////
		for (int i = 0; i < DataList.size(); i++)
		{
			// stbiでテクスチャバイナリを解析してピクセルデータを取得する
			stbi_uc* stbi_pixelData = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(&DataList[i][0]), static_cast<int>(DataList[i].size()), &m_Width, &m_Height, &m_NumOfChannels, STBI_rgb_alpha);

			// stbiから取得したピクセルデータを扱いやすいデータにコピーしておく
			int pixelSize = m_Width * m_Height * 4;
			std::vector<unsigned char> pixelData(pixelSize);
			std::memcpy(&pixelData[0], stbi_pixelData, pixelSize);

			// stbiのメモリを解放
			stbi_image_free(stbi_pixelData);

			//
			pixelDataList.push_back(pixelData);
			pixelSizeList.push_back(pixelSize);
		}
#endif // USE_PNG_PARSER

		// データをまとめる
		std::vector<unsigned char> pixelData;
		int pixelSize = 0;

		for (int i = 0; i < pixelDataList.size(); i++)
		{
			int ByteSize = pixelSizeList[i];
			int ByteOffset = static_cast<int>(pixelData.size());

			pixelData.resize(ByteOffset + ByteSize);

			std::memcpy(&pixelData[ByteOffset], &pixelDataList[i][0], ByteSize);

			pixelSize += ByteSize;
		}

		// MipCountを計算
		int CurrentSize = (m_Width < m_Height) ? m_Width : m_Height;
		int MipCount = 0;

		if (IsPowerOfTwo(CurrentSize))
		{
			while (CurrentSize != 1)
			{
				CurrentSize /= 2;
				MipCount++;
			}

			m_MipCount = static_cast<float>(MipCount);
		}

		// APIにデータを渡す
		if (!Create(pixelData, pixelSize)) return false;

		return true;
	}

	bool CTexture::Create(const std::vector<unsigned char>& Data, int Width, int Height, int NumOfChannels, api::ERenderPassFormat RenderPassFormat)
	{
		m_Width = Width;
		m_Height = Height;
		m_NumOfChannels = NumOfChannels;
		m_RenderPassFormat = RenderPassFormat;

		int pixelSize = static_cast<int>(Data.size());

		if (!Create(Data, pixelSize)) return false;

		return true;
	}

	bool CTexture::Create(const std::vector<unsigned char>& pixelData, int pixelSize)
	{
		return true;
	}
#endif // USE_TEXTURE_LOADER

	bool CTexture::IsPowerOfTwo(int n)
	{
		if (n == 0)
			return false;
		while (n != 1) {
			if (n % 2 != 0)
				return 0;
			n = n / 2;
		}
		return true;
	}
}