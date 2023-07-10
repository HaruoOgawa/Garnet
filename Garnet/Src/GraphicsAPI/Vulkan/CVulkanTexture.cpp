#ifndef __DAWN__
#include "CVulkanTexture.h"
#include "CVulkanAPI.h"
namespace api
{
	CVulkanTexture::CVulkanTexture(api::CVulkanAPI* pGraphicsAPI, bool UseMipMap):
		CTexture(UseMipMap),
		m_pGraphicsAPI(pGraphicsAPI),
		m_TextureImage(nullptr),
		m_TextureImageMemory(nullptr),
		m_TextureImageView(nullptr),
		m_TextureSampler(nullptr)
	{
	}

	CVulkanTexture::~CVulkanTexture()
	{
		// テクスチャサンプラーを破棄
		if (m_TextureSampler)
		{
			vkDestroySampler(m_pGraphicsAPI->GetLogicalDevice(), m_TextureSampler, nullptr);
			m_TextureSampler = nullptr;
		}


		// テクスチャ用のイメージビューの破棄
		if (m_TextureImageView)
		{
			vkDestroyImageView(m_pGraphicsAPI->GetLogicalDevice(), m_TextureImageView, nullptr);
			m_TextureImageView = nullptr;
		}


		// テクスチャイメージの破棄
		if (m_TextureImage)
		{
			vkDestroyImage(m_pGraphicsAPI->GetLogicalDevice(), m_TextureImage, nullptr);
			m_TextureImage = nullptr;
		}

		if (m_TextureImageMemory)
		{
			vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), m_TextureImageMemory, nullptr);
			m_TextureImageMemory = nullptr;
		}
	}

	const VkImage& CVulkanTexture::GetTextureImage() const
	{
		return m_TextureImage;
	}

	const VkDeviceMemory& CVulkanTexture::GetTextureImageMemory() const
	{
		return m_TextureImageMemory;
	}

	const VkImageView& CVulkanTexture::GetTextureImageView() const
	{
		return m_TextureImageView;
	}

	const VkSampler& CVulkanTexture::GetTextureSampler() const
	{
		return m_TextureSampler;
	}

	bool CVulkanTexture::CreateFrameTexture(int Width, int Height, api::ERenderPassFormat RenderPassFormat)
	{
		m_Width = Width;
		m_Height = Height;
		m_RenderPassFormat = RenderPassFormat;

		VkFormat ImageFormat = VK_FORMAT_UNDEFINED;
		VkImageUsageFlags Usage;

		switch (RenderPassFormat)
		{
		case api::ERenderPassFormat::COLOR_RENDERPASS:
			ImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
			Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			break;
		case api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS:
			ImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
			Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			break;
		case api::ERenderPassFormat::DEPTH_RENDERPASS:
		case api::ERenderPassFormat::DEPTH_FLOAT_RENDERPASS:
			ImageFormat = m_pGraphicsAPI->FindDepthFormat();
			Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			break;
		default:
			ImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
			Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			break;
		}

		if (!CreateFrameTextureImage(ImageFormat, Usage)) return false; // テクスチャイメージの生成
		if (!CreateTextureImageView(ImageFormat)) return false;// シェーダーで取り扱う用のImageViewを作成(イメージマネージャーみたいなやつかな)
		if (!CreateTextureSampler()) return false; // テクスチャサンプラーを作成.サンプラーとはテクスチャデータをフラグメント(3Dモデル)に合うように調整する機構

		return true;
	}

#ifdef USE_TEXTURE_LOADER
	bool CVulkanTexture::Create(const std::vector<unsigned char>& pixelData, int pixelSize)
	{
		VkFormat ImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
		m_RenderPassFormat = api::ERenderPassFormat::COLOR_RENDERPASS;

		// Texture Buffer
		if (!CreateTextureImage(pixelData, pixelSize, ImageFormat)) return false; // テクスチャイメージの生成
		if (!CreateTextureImageView(ImageFormat)) return false;// シェーダーで取り扱う用のImageViewを作成(イメージマネージャーみたいなやつかな)
		if (!CreateTextureSampler()) return false; // テクスチャサンプラーを作成.サンプラーとはテクスチャデータをフラグメント(3Dモデル)に合うように調整する機構

		return true;
	}
#endif
	// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CVulkanTexture::CreateFrameTextureImage(VkFormat ImageFormat, VkImageUsageFlags Usage)
	{
		// テクスチャイメージオブジェクトを生成
		m_pGraphicsAPI->CreateImage(m_Width, m_Height, ImageFormat, VK_IMAGE_TILING_OPTIMAL, Usage,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, m_TextureType, m_MipCount, m_UseMipMap);

		return true;
	}

	bool CVulkanTexture::CreateTextureImage(const std::vector<unsigned char>& pixelData, int pixelSize, VkFormat ImageFormat)
	{
		// テクスチャイメージオブジェクトを生成
		m_pGraphicsAPI->CreateImage(m_Width, m_Height, ImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, m_TextureType, m_MipCount, m_UseMipMap);

		// テクスチャイメージのステージングバッファを作成
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		m_pGraphicsAPI->CreateBuffer(pixelSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);

		void* data; // <-- dataにGPUへの入り口のポインタが渡される(あとで閉じられる) -> そこにテクスチャデータをコピーする(GPUのバッファに渡される)
		vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, 0, pixelSize, 0, &data);
		std::memcpy(data, &pixelData[0], static_cast<size_t>(pixelSize));
		vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory);

		// イメージテクスチャのレイアウトを別形式へ移行する --> バッファにコピー可な形式に変換
		m_pGraphicsAPI->TransitionImageLayout(m_TextureImage, ImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipCount, m_UseMipMap);

		// ステージングバッファのデータをテクスチャイメージへコピーする
		m_pGraphicsAPI->CopyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height), m_TextureType, m_MipCount, m_UseMipMap, m_HasMipData);

		// イメージテクスチャのレイアウトを別形式へ移行する --> シェーダーで読み込み可な形式に変換
		m_pGraphicsAPI->TransitionImageLayout(m_TextureImage, ImageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_MipCount, m_UseMipMap);

		// ステージングバッファの破棄
		vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, nullptr);

		// 元のデータがミップマップデータを持っていないなら動的生成する
		if (!m_HasMipData && m_UseMipMap)
		{
			if (m_TextureType == graphics::ETextureType::TEXTURE_2D)
			{
				if (!GenerateMipMap(0)) return false;
			}
			else if (m_TextureType == graphics::ETextureType::TEXTURE_CUBE)
			{
				for (uint32_t layer = 0; layer < 6; layer++)
				{
					if (!GenerateMipMap(layer)) return false;
				}
			}
		}

		return true;
	}

	bool CVulkanTexture::CreateTextureImageView(VkFormat ImageFormat)
	{
		m_TextureImageView = m_pGraphicsAPI->CreateImageView(m_TextureImage, ImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, m_TextureType, m_MipCount, m_UseMipMap);

		return true;
	}

	bool CVulkanTexture::CreateTextureSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(m_pGraphicsAPI->GetPhysicalDevice(), &properties);
		samplerInfo.anisotropyEnable = VK_TRUE; // 異方性フィルタリング --> 遠くの方のテクスチャがぼけてしまうのを調整する機
		samplerInfo.maxAnisotropy = static_cast<float>(properties.limits.maxSamplerAllocationCount);

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_TRUE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = m_MipCount;

		if (vkCreateSampler(m_pGraphicsAPI->GetLogicalDevice(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	// Helper Function ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CVulkanTexture::GenerateMipMap(uint32_t layer)
	{
		// コマンドバッファの記録開始
		VkCommandBuffer commandBuffer = m_pGraphicsAPI->BeginSingleTimeCommands();

		// Vk〇〇MemoryBarrierってよく出てくるけどなんだ？
		// たぶんコマンド実行中にその専用のメモリを確実に事前確保しておくための記述
		// そのパラメーター = メモリレイアウトの指定
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = m_TextureImage;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // キューファミリは無視
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // キューファミリは無視
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // メモリレイアウトを整形するためのフラグ？
		barrier.subresourceRange.layerCount = 1; // 一度に計算するレイヤー数. 1つずつ処理する
		barrier.subresourceRange.baseArrayLayer = layer; // 処理の基準レイヤー. イメージ配列のレイヤーインデックス. Cubemapの場合ここが変わる. 
		barrier.subresourceRange.levelCount = 1; // 一度に処理するミップの数

		for (uint32_t level = 1; level < m_MipCount; level++)
		{
			barrier.subresourceRange.baseMipLevel = level - 1; // ミップレベル. 設定値はインデックス
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			// メモリバリアコマンドを発行
			vkCmdPipelineBarrier(
				commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);

			// 縮小コマンドの設定
			VkImageBlit blit{};
			// Srcの設定
			blit.srcOffsets[0] = { 0, 0, static_cast<int>(layer) }; // srcOffsets, dstOffsetsが大きさ2の配列になっているのは、ピクセル領域の使用範囲を決めるためである
			blit.srcOffsets[1] = { m_Width >> (level - 1) , m_Height >> (level - 1), static_cast<int>(layer) }; // Blit用に入ってきたイメージのピクセル範囲
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = level - 1; // ミップレベル. 設定値はインデックス
			blit.srcSubresource.baseArrayLayer = layer; // イメージ配列のレイヤーインデックス
			blit.srcSubresource.layerCount = 1;

			// Dstの設定
			blit.dstOffsets[0] = { 0, 0, static_cast<int>(layer) }; // 書き出すイメージのピクセル範囲
			blit.dstOffsets[1] = { m_Width >> level , m_Height >> level, static_cast<int>(layer) };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = level; // ミップレベル. 設定値はインデックス
			blit.dstSubresource.baseArrayLayer = layer; // イメージ配列のレイヤーインデックス
			blit.dstSubresource.layerCount = 1;

			// Imageを縮小するコマンドを発行
			vkCmdBlitImage(
				commandBuffer,
				m_TextureImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				m_TextureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR
			);

			// メモリをシェーダーリードオンリーにする
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(
				commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);
		}
		
		// 記録終了
		m_pGraphicsAPI->EndSingleTimeCommands(commandBuffer);

		return true;
	}
}
#endif