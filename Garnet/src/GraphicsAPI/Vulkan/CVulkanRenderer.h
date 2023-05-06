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

#include <vector>
#include <array>
#include <string>

#include "../../Interface/IRenderer.h"

// はぇー時間を管理するstd標準ライブラリ
#include <chrono>

namespace api { class CVulkanAPI; }

namespace renderer
{
	class CRendererCreateInfo;

	class CVulkanRenderer : public IRenderer
	{
		// API
		api::CVulkanAPI* m_pGraphicsAPI;

		// Uniform Buffer Object
		VkDescriptorSetLayout m_DescriptorSetLayout;
		std::vector<VkBuffer> m_UniformBuffers;
		std::vector<VkDeviceMemory> m_UniformBuffersMemory;
		std::vector<void*> m_UniformBuffersMapped;
		VkDescriptorPool m_DescriptorPool;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		// Pipeline
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;

		// Texture Image
		bool m_UseMainTexture;
		VkImage m_TextureImage;
		VkDeviceMemory m_TextureImageMemory;
		VkImageView m_TextureImageView; // シェーダーでテクスチャを取り扱う用のImageView
		VkSampler m_TextureSampler;

	private:
		// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateDescriptorSetLayout(const CRendererCreateInfo& createInfo);
		bool CreateTextureImage(const CRendererCreateInfo& createInfo);
		bool CreateTextureImageView(const CRendererCreateInfo& createInfo);
		bool CreateTextureSampler(const CRendererCreateInfo& createInfo);
		
		bool CreateUniformBuffers(const CRendererCreateInfo& createInfo);
		bool CreateDescriptorPool(const CRendererCreateInfo& createInfo);
		bool CreateDescriptorSets(const CRendererCreateInfo& createInfo);
		bool CreateGraphicsPipeline(const CRendererCreateInfo& createInfo);

		void UpdateUniformBuffer(uint32_t CurrentImage, float SecondsTime);

		// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Shader
		bool CreateShaderModule(VkShaderModule& shaderModule, const std::string& code);
		VkFormat GetVertexFormat(int Dimention);
	public:
		CVulkanRenderer();
		virtual ~CVulkanRenderer();

		void Release();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo) override;
		bool Update(float SecondsTime) override;
		bool Draw() override;
	};
}
#endif