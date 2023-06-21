#ifdef __DAWN__
#include "CWebGPUTexture.h"
#include "CWebGPUAPI.h"
namespace api
{
	CWebGPUTexture::CWebGPUTexture(api::CWebGPUAPI* pGraphicsAPI):
		CTexture(),
		m_pGraphicsAPI(pGraphicsAPI),
		m_TextureImageView(nullptr),
		m_TextureSampler(nullptr)
	{
	}

	CWebGPUTexture::~CWebGPUTexture()
	{
	}

#ifdef USE_TEXTURE_LOADER
	bool CWebGPUTexture::Create(const std::vector<unsigned char>& pixelData, int pixelSize)
	{
		if (!CreateTextureImageView(pixelData, pixelSize)) return false; // ImageViewを生成
		if (!CreateTextureSampler()) return false; // Samplerを生成

		return true;
	}
#endif

	const WGPUTextureView& CWebGPUTexture::GetTextureImageView() const
	{
		return m_TextureImageView;
	}

	const WGPUSampler& CWebGPUTexture::GetTextureSampler() const
	{
		return m_TextureSampler;
	}

	bool CWebGPUTexture::CreateTextureImageView(const std::vector<unsigned char>& pixelData, int pixelSize)
	{
		WGPUTextureFormat textureFormat = WGPUTextureFormat_RGBA8Unorm;

		unsigned int TexCount = (m_TextureType == graphics::ETextureType::TEXTURE_CUBE) ? 6 : 1;

		// Textureを生成
		WGPUTextureDescriptor textureDesc{};
		textureDesc.nextInChain = nullptr;
		textureDesc.dimension = WGPUTextureDimension_2D;
		textureDesc.format = textureFormat;
		textureDesc.mipLevelCount = 1;
		textureDesc.sampleCount = 1;
		textureDesc.size = { static_cast<unsigned int>(m_Width), static_cast<unsigned int>(m_Height), TexCount };
		textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		WGPUTexture texture = wgpuDeviceCreateTexture(m_pGraphicsAPI->GetLogicalDevice(), &textureDesc);

		// データをテクスチャオブジェクトに書き込む
		WGPUImageCopyTexture destination; // destinationはテクスチャの様式を設定
		destination.nextInChain = nullptr;
		destination.texture = texture; // テクスチャオブジェクト
		destination.mipLevel = 0; // ミップマップレベル
		destination.origin = { 0, 0, 0 }; // テクスチャの原点の位置
		destination.aspect = WGPUTextureAspect_All;

		WGPUTextureDataLayout source{}; // sourceはバッファからの読み取り方法を示す
		source.nextInChain = nullptr;
		source.offset = 0;
		source.bytesPerRow = 4 * m_Width;
		source.rowsPerImage = m_Height;

		if (m_TextureType == graphics::ETextureType::TEXTURE_CUBE)
		{
			// TEXTURE_CUBE
			WGPUExtent3D singleLayerSize = { static_cast<uint32_t>(m_Width) , static_cast<uint32_t>(m_Height) , 1 };
			for (unsigned int layer = 0; layer < 6; layer++)
			{
				size_t TexSize = static_cast<size_t>(m_Width * m_Height * 4);
				size_t byteOffset = TexSize * layer;
				destination.origin = { 0, 0, layer }; // CubemapはZ軸方向に積み重なったTexture2D Arrayとみる

				wgpuQueueWriteTexture(m_pGraphicsAPI->GetQueue(), &destination, &pixelData[byteOffset], TexSize, &source, &singleLayerSize);
			}
		}
		else
		{
			// TEXTURE_2D
			wgpuQueueWriteTexture(m_pGraphicsAPI->GetQueue(), &destination, &pixelData[0], pixelSize, &source, &textureDesc.size);
		}

		// TextureViewを生成
		WGPUTextureViewDescriptor textureViewDesc{};
		textureViewDesc.nextInChain = nullptr;
		textureViewDesc.aspect = WGPUTextureAspect_All;
		textureViewDesc.baseArrayLayer = 0;
		textureViewDesc.arrayLayerCount = (m_TextureType == graphics::ETextureType::TEXTURE_CUBE)? 6 : 1;
		textureViewDesc.baseMipLevel = 0;
		textureViewDesc.mipLevelCount = 1;
		textureViewDesc.dimension = (m_TextureType == graphics::ETextureType::TEXTURE_CUBE)? WGPUTextureViewDimension_Cube : WGPUTextureViewDimension_2D;
		textureViewDesc.format = textureFormat;
		
		m_TextureImageView = wgpuTextureCreateView(texture, &textureViewDesc);

		return true;
	}

	bool CWebGPUTexture::CreateTextureSampler()
	{
		WGPUSamplerDescriptor samplerDesc{};
		samplerDesc.addressModeU = WGPUAddressMode_Repeat;
		samplerDesc.addressModeV = WGPUAddressMode_Repeat;
		samplerDesc.addressModeW = WGPUAddressMode_Repeat;
		samplerDesc.magFilter = WGPUFilterMode_Linear;
		samplerDesc.minFilter = WGPUFilterMode_Linear;
		samplerDesc.lodMinClamp = 0.0f;
		samplerDesc.lodMaxClamp = m_MipCount;
		samplerDesc.compare = WGPUCompareFunction_Undefined;
		samplerDesc.maxAnisotropy = 0;

		m_TextureSampler = wgpuDeviceCreateSampler(m_pGraphicsAPI->GetLogicalDevice(), &samplerDesc);

		return true;
	}
}
#endif