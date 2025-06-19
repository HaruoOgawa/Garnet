#pragma once
#ifdef USE_VULKAN
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

		// ImGuiが使用
		VkDescriptorSetLayout m_GUIDescriptorSetLayout;
		VkDescriptorPool m_GUIDescriptorPool;
		VkDescriptorSet m_GUIDescriptorSet;
	private:
		// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateFrameTextureImage(VkFormat ImageFormat, VkImageUsageFlags Usage, VkMemoryPropertyFlags properties, VkSampleCountFlagBits msaaSamples);
		bool CreateComputeTextureImage(VkFormat ImageFormat, VkImageUsageFlags Usage, VkMemoryPropertyFlags properties, VkSampleCountFlagBits msaaSamples);
		bool CreateTextureImage(const std::vector<unsigned char>& pixelData, int pixelSize, VkFormat ImageFormat);
		bool CreateTextureImageView(VkFormat ImageFormat, bool UseColor, bool UseDepth, bool UseStencil);
		bool CreateTextureSampler();

		// ImGuiが使用
		bool CreateGUIDescriptorSetLayout();
		bool CreateGUIDescriptorPool();
		bool CreateGUIDescriptorSet();

		// Helper Function ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool GenerateMipMap(uint32_t layer);
	public:
		CVulkanTexture(api::CVulkanAPI* pGraphicsAPI, bool UseMipMap, const graphics::STextureSamplerParam& SamplerParam);
		virtual ~CVulkanTexture();

		virtual bool CreateFrameTexture(int Width, int Height, api::ERenderPassFormat RenderPassFormat, int AASampleNum, bool ReadOnShader) override;
		virtual bool CreateComputeTexture(int Width, int Height) override;
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) override;

		const VkImage& GetTextureImage() const;
		const VkDeviceMemory& GetTextureImageMemory() const;
		const VkImageView& GetTextureImageView() const;
		const VkSampler& GetTextureSampler() const;

		//
		VkDescriptorSet GetGUIDescriptorSet();
	};
}
#endif