#ifdef USE_WEBGPU
#include "CWebGPUTexture.h"
#include "CWebGPUAPI.h"
namespace api
{
	CWebGPUTexture::CWebGPUTexture(api::CWebGPUAPI* pGraphicsAPI, bool UseMipMap):
		CTexture(UseMipMap),
		m_pGraphicsAPI(pGraphicsAPI),
		m_TextureImageView(nullptr),
		m_TextureSampler(nullptr)
	{
	}

	CWebGPUTexture::~CWebGPUTexture()
	{
	}

	bool CWebGPUTexture::CreateFrameTexture(int Width, int Height, api::ERenderPassFormat RenderPassFormat)
	{
		m_Width = Width;
		m_Height = Height;
		m_RenderPassFormat = RenderPassFormat;

		if (!CreateFrameTextureImageView(RenderPassFormat)) return false; // Samplerを生成
		if (!CreateTextureSampler()) return false; // Samplerを生成

		return true;
	}

#ifdef USE_TEXTURE_LOADER
	bool CWebGPUTexture::Create(const std::vector<unsigned char>& OriginalPixels, int pixelSize)
	{
		m_RenderPassFormat = api::ERenderPassFormat::COLOR_RENDERPASS;

		if (!CreateTextureImageView(OriginalPixels, pixelSize)) return false; // ImageViewを生成
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

	bool CWebGPUTexture::CreateFrameTextureImageView(api::ERenderPassFormat RenderPassFormat)
	{
		// レンダーパスに使用するカラーバッファのTextureViewフォーマットはWGPUTextureFormat_BGRA8Unormのみ対応している. RGBAの順番じゃないことに要注意!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// https://eliemichel.github.io/LearnWebGPU/getting-started/first-color.html#:~:text=WGPUTextureFormat_BGRA8Unorm
		WGPUTextureFormat textureFormat = WGPUTextureFormat_Undefined;
		WGPUTextureAspect aspect = WGPUTextureAspect_All;

		switch (RenderPassFormat)
		{
		case api::ERenderPassFormat::COLOR_RENDERPASS:
			textureFormat = WGPUTextureFormat_BGRA8Unorm;
			break;
		case api::ERenderPassFormat::DEPTH_RENDERPASS:
			textureFormat = WGPUTextureFormat_Depth24Plus;
			aspect = WGPUTextureAspect_DepthOnly;
			break;
		case api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS:
			textureFormat = WGPUTextureFormat_BGRA8Unorm;
			break;
		case api::ERenderPassFormat::DEPTH_FLOAT_RENDERPASS:
			textureFormat = WGPUTextureFormat_Depth24Plus;
			aspect = WGPUTextureAspect_DepthOnly;
			break;
		default:
			textureFormat = WGPUTextureFormat_BGRA8Unorm;
			break;
		}

		// Textureを生成
		WGPUTextureDescriptor textureDesc{};
		textureDesc.nextInChain = nullptr;
		textureDesc.dimension = WGPUTextureDimension_2D;
		textureDesc.format = textureFormat;
		textureDesc.mipLevelCount = 1;
		textureDesc.sampleCount = 1;
		textureDesc.size = { static_cast<unsigned int>(m_Width), static_cast<unsigned int>(m_Height), 1 };
		textureDesc.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		WGPUTexture texture = wgpuDeviceCreateTexture(m_pGraphicsAPI->GetLogicalDevice(), &textureDesc);

		// TextureViewを生成
		WGPUTextureViewDescriptor textureViewDesc{};
		textureViewDesc.nextInChain = nullptr;
		textureViewDesc.aspect = aspect;
		textureViewDesc.baseArrayLayer = 0;
		textureViewDesc.arrayLayerCount = 1;
		textureViewDesc.baseMipLevel = 0;
		textureViewDesc.mipLevelCount = 1;
		textureViewDesc.dimension = WGPUTextureViewDimension_2D;
		textureViewDesc.format = textureFormat;

		m_TextureImageView = wgpuTextureCreateView(texture, &textureViewDesc);

		return true;
	}

	bool CWebGPUTexture::CreateTextureImageView(const std::vector<unsigned char>& OriginalPixels, int pixelSize)
	{
		WGPUTextureFormat textureFormat = WGPUTextureFormat_RGBA8Unorm;
		
		unsigned int TexCount = (m_TextureType == graphics::ETextureType::TEXTURE_CUBE) ? 6 : 1;

		// Textureを生成
		WGPUTextureDescriptor textureDesc{};
		textureDesc.nextInChain = nullptr;
		textureDesc.dimension = WGPUTextureDimension_2D;
		textureDesc.format = textureFormat;
		textureDesc.mipLevelCount = (m_UseMipMap) ? static_cast<uint32_t>(m_MipCount) : 1;
		textureDesc.sampleCount = 1;
		textureDesc.size = { static_cast<unsigned int>(m_Width), static_cast<unsigned int>(m_Height), TexCount };
		textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		WGPUTexture texture = wgpuDeviceCreateTexture(m_pGraphicsAPI->GetLogicalDevice(), &textureDesc);

		// データをテクスチャオブジェクトに書き込む
		if (m_TextureType == graphics::ETextureType::TEXTURE_CUBE)
		{
			if (m_UseMipMap)
			{
				// TEXTURE_CUBE
				for (unsigned int layer = 0; layer < 6; layer++)
				{
					uint32_t mipW = static_cast<uint32_t>(m_Width);
					uint32_t mipH = static_cast<uint32_t>(m_Height);

					std::vector<unsigned char> prevPixels;

					for (uint32_t level = 0; level < static_cast<uint32_t>(m_MipCount); level++)
					{
						WGPUExtent3D singleLayerSize = { mipW , mipH , 1 };

						//
						WGPUImageCopyTexture destination; // destinationはテクスチャの様式を設定
						destination.nextInChain = nullptr;
						destination.texture = texture; // テクスチャオブジェクト
						destination.origin = { 0, 0, layer }; // CubemapはZ軸方向に積み重なったTexture2D Arrayとみる
						destination.mipLevel = level;
						destination.aspect = WGPUTextureAspect_All;

						WGPUTextureDataLayout source{}; // sourceはバッファからの読み取り方法を示す
						source.nextInChain = nullptr;
						source.offset = 0;
						source.bytesPerRow = 4 * mipW;
						source.rowsPerImage = mipH;

						size_t TexSize = static_cast<size_t>((mipW * 4) * mipH);
						size_t byteOffset = TexSize * layer;

						if (level == 0)
						{
							// 最初のミップレベルなので元のピクセルデータを使用する
							
							std::vector<unsigned char> pixels(TexSize);
							std::memcpy(&pixels[0], &OriginalPixels[byteOffset], TexSize);

							// データをリソースにコピー
							wgpuQueueWriteTexture(m_pGraphicsAPI->GetQueue(), &destination, &OriginalPixels[byteOffset], TexSize, &source, &singleLayerSize);

							// 次の計算に使うピクセルを更新
							prevPixels = pixels;
						}
						else
						{
							// 前のピクセル4つの相加平均で次の1つのピクセルを求めてテクスチャを縮小させる
							// https://eliemichel.github.io/LearnWebGPU/basic-3d-rendering/texturing/sampler.html#mip-mapping
							std::vector<unsigned char> pixels(TexSize);

							// ピクセルを縮小(そのうちにCPU演算からGPGPUに移行したいかも・・・)
							if (!ComputeShrinkPixels(prevPixels, pixels, mipW, mipH)) return false;

							// データをリソースにコピー
							wgpuQueueWriteTexture(m_pGraphicsAPI->GetQueue(), &destination, &pixels[0], pixels.size(), &source, &singleLayerSize);

							// 次の計算に使うピクセルを更新
							prevPixels = pixels;
						}

						// サイズを縮小
						mipW = mipW / 2;
						mipH = mipH / 2;
					}
				}
			}
			else
			{
				//
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

				// TEXTURE_CUBE
				WGPUExtent3D singleLayerSize = { static_cast<uint32_t>(m_Width) , static_cast<uint32_t>(m_Height) , 1 };
				for (unsigned int layer = 0; layer < 6; layer++)
				{
					size_t TexSize = static_cast<size_t>((m_Width * 4) * m_Height);
					size_t byteOffset = TexSize * layer;
					destination.origin = { 0, 0, layer }; // CubemapはZ軸方向に積み重なったTexture2D Arrayとみる

					wgpuQueueWriteTexture(m_pGraphicsAPI->GetQueue(), &destination, &OriginalPixels[byteOffset], TexSize, &source, &singleLayerSize);
				}
			}
		}
		else
		{
			//
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

			// TEXTURE_2D
			wgpuQueueWriteTexture(m_pGraphicsAPI->GetQueue(), &destination, &OriginalPixels[0], pixelSize, &source, &textureDesc.size);
		}

		// TextureViewを生成
		WGPUTextureViewDescriptor textureViewDesc{};
		textureViewDesc.nextInChain = nullptr;
		textureViewDesc.aspect = WGPUTextureAspect_All;
		textureViewDesc.baseArrayLayer = 0;
		textureViewDesc.arrayLayerCount = (m_TextureType == graphics::ETextureType::TEXTURE_CUBE)? 6 : 1;
		textureViewDesc.baseMipLevel = 0;
		textureViewDesc.mipLevelCount = (m_UseMipMap) ? static_cast<uint32_t>(m_MipCount) : 1;
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

	// Helper Function ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CWebGPUTexture::ComputeShrinkPixels(const std::vector<unsigned char>& SrcPixels, std::vector<unsigned char>& DstPixles, uint32_t w, uint32_t h)
	{
		uint32_t prevW = w * 2 * 4; // RGBAだから4倍
		uint32_t prevH = h * 2;
		uint32_t piexlIndex = 0;

		for (uint32_t y = 0; y < prevH; y += 2)
		{
			for (uint32_t x = 0; x < prevW; x += 8)
			{
				// 4セットのピクセルを取得
				// これはテクスチャサイズが4x4だった時の例.
				// 4セットで平均をとり、縮小された一つのピクセルを計算する
				// r, g, b, a (p00)| r, g, b, a(p10)| r, g, b, a | r, g, b, a|
				// r, g, b, a (p10)| r, g, b, a(p11)| r, g, b, a | r, g, b, a|
				// r, g, b, a      | r, g, b, a     | r, g, b, a | r, g, b, a|
				// r, g, b, a      | r, g, b, a     | r, g, b, a | r, g, b, a|
				const unsigned char* p00 = &SrcPixels[(prevW * (y + 0)) + (x + 4 * 0)];
				const unsigned char* p10 = &SrcPixels[(prevW * (y + 0)) + (x + 4 * 1)];
				const unsigned char* p01 = &SrcPixels[(prevW * (y + 1)) + (x + 4 * 0)];
				const unsigned char* p11 = &SrcPixels[(prevW * (y + 1)) + (x + 4 * 1)];

				// 平均を求めて値を渡す
				DstPixles[piexlIndex * 4 + 0] = (p00[0] + p10[0] + p01[0] + p11[0]) / 4;
				DstPixles[piexlIndex * 4 + 1] = (p00[1] + p10[1] + p01[1] + p11[1]) / 4;
				DstPixles[piexlIndex * 4 + 2] = (p00[2] + p10[2] + p01[2] + p11[2]) / 4;
				DstPixles[piexlIndex * 4 + 3] = (p00[3] + p10[3] + p01[3] + p11[3]) / 4;

				// 最後にインデックスを更新する
				piexlIndex++;
			}
		}

		return true;
	}
}
#endif