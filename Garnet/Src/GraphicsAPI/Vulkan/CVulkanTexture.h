#pragma once
#ifndef __DAWN__
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <glfw3.h>
#include <glfw3native.h>

#include "../../Graphics/CTexture.h"

namespace api
{
	class CVulkanAPI;

	class CVulkanTexture : public graphics::CTexture
	{
		// API
		api::CVulkanAPI* m_pGraphicsAPI;

		// Texture Image
		VkImage m_TextureImage;
		VkDeviceMemory m_TextureImageMemory;
		VkImageView m_TextureImageView; // シェーダーでテクスチャを取り扱う用のImageView
		VkSampler m_TextureSampler;
	private:
		// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateTextureImage(const std::vector<unsigned char>& pixelData, int pixelSize);
		bool CreateTextureImageView();
		bool CreateTextureSampler();

		// Helper Function ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool GenerateMipMap(uint32_t layer);
	public:
		CVulkanTexture(api::CVulkanAPI* pGraphicsAPI, bool UseMipMap);
		virtual ~CVulkanTexture();

#ifdef USE_TEXTURE_LOADER
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) override;
#endif
		const VkImage& GetTextureImage() const;
		const VkDeviceMemory& GetTextureImageMemory() const;
		const VkImageView& GetTextureImageView() const;
		const VkSampler& GetTextureSampler() const;
	};
}
#endif