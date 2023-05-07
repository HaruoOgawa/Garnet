#pragma once
#ifndef __DAWN__
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <glfw3.h>
#include <glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../Graphics/CMaterial.h"

namespace graphics { class CMaterialCreateInfo; }

namespace api
{
	class CVulkanAPI;

	class CVulkanMaterial : public graphics::CMaterial
	{
		// API
		api::CVulkanAPI* m_pGraphicsAPI;

		// Shader
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;

		// Uniform Buffer Object
		VkDescriptorSetLayout m_DescriptorSetLayout;
		std::vector<std::vector<VkBuffer>> m_UniformBuffersList;
		std::vector<std::vector<VkDeviceMemory>> m_UniformBuffersMemoryList;
		std::vector<std::vector<void*>> m_UniformBuffersMappedList;
		VkDescriptorPool m_DescriptorPool;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		// Texture Image
		bool m_UseMainTexture;
		VkImage m_TextureImage;
		VkDeviceMemory m_TextureImageMemory;
		VkImageView m_TextureImageView; // シェーダーでテクスチャを取り扱う用のImageView
		VkSampler m_TextureSampler;
	private:
		// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateShaderStages(const graphics::CMaterialCreateInfo& createInfo);
		
		bool CreateTextureImage(const graphics::CMaterialCreateInfo& createInfo);
		bool CreateTextureImageView(const graphics::CMaterialCreateInfo& createInfo);
		bool CreateTextureSampler(const graphics::CMaterialCreateInfo& createInfo);

		bool CreateDescriptorSetLayout(const graphics::CMaterialCreateInfo& createInfo);
		bool CreateUniformBuffers(const graphics::CMaterialCreateInfo& createInfo);
		bool CreateDescriptorPool(const graphics::CMaterialCreateInfo& createInfo);
		bool CreateDescriptorSets(const graphics::CMaterialCreateInfo& createInfo);

		void UpdateUniformBuffer(uint32_t CurrentImage, float SecondsTime);

		void Release();

		// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateShaderModule(VkShaderModule& shaderModule, const std::string& code);
	public:
		CVulkanMaterial();
		virtual ~CVulkanMaterial();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const graphics::CMaterialCreateInfo& createInfo) override;
		bool Update(float SecondsTime) override;

		const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages()const { return m_ShaderStages; }
		const VkDescriptorSetLayout& GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
		const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return m_DescriptorSets; }
	};
}
#endif