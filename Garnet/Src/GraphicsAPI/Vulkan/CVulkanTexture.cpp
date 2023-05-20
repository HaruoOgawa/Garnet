#ifndef __DAWN__
#include "CVulkanTexture.h"
#include "CVulkanAPI.h"
namespace api
{
	CVulkanTexture::CVulkanTexture(api::CVulkanAPI* pGraphicsAPI):
		CTexture(),
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

#ifdef USE_TEXTURE_LOADER
	bool CVulkanTexture::Create(const std::vector<unsigned char>& pixelData, int pixelSize)
	{
		// Texture Buffer
		if (!CreateTextureImage(pixelData, pixelSize)) return false; // テクスチャイメージの生成
		if (!CreateTextureImageView()) return false;// シェーダーで取り扱う用のImageViewを作成(イメージマネージャーみたいなやつかな)
		if (!CreateTextureSampler()) return false; // テクスチャサンプラーを作成.サンプラーとはテクスチャデータをフラグメント(3Dモデル)に合うように調整する機構

		return true;
	}
#endif
	// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CVulkanTexture::CreateTextureImage(const std::vector<unsigned char>& pixelData, int pixelSize)
	{
		// テクスチャイメージのステージングバッファを作成
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		m_pGraphicsAPI->CreateBuffer(pixelSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);

		void* data; // <-- dataにGPUへの入り口のポインタが渡される(あとで閉じられる) -> そこにテクスチャデータをコピーする(GPUのバッファに渡される)
		vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, 0, pixelSize, 0, &data);
		std::memcpy(data, &pixelData[0], static_cast<size_t>(pixelSize));
		vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory);

		// テクスチャイメージオブジェクトを生成
		m_pGraphicsAPI->CreateImage(m_Width, m_Height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory);

		// イメージテクスチャのレイアウトを別形式へ移行する --> バッファにコピー可な形式に変換
		m_pGraphicsAPI->TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		// ステージングバッファのデータをテクスチャイメージへコピーする
		m_pGraphicsAPI->CopyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));

		// イメージテクスチャのレイアウトを別形式へ移行する --> シェーダーで読み込み可な形式に変換
		m_pGraphicsAPI->TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		// ステージングバッファの破棄
		vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, nullptr);

		return true;
	}

	bool CVulkanTexture::CreateTextureImageView()
	{
		m_TextureImageView = m_pGraphicsAPI->CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

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
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(m_pGraphicsAPI->GetLogicalDevice(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}
}
#endif