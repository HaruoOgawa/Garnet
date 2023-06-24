#pragma once
#ifndef __DAWN__
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <memory>

#include <glfw3.h>
#include <glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../Graphics/CMaterial.h"

namespace graphics { class CMaterialCreateInfo; }

namespace api
{
	class CVulkanAPI;
	class CVulkanTexture;

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

		std::vector<std::vector<VkBuffer>> m_VKUniformBufferList;
		std::vector<std::vector<VkDeviceMemory>> m_VKUniformBufferMemoryList;
		std::vector<std::vector<uint32_t>> m_VKUniformBufferSizeList;

		VkDescriptorPool m_DescriptorPool;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		// Texture
		std::shared_ptr<CVulkanTexture> m_EmptyTexture;
	private:
		// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateShaderStages(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		
		bool CreateDescriptorSetLayout(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateUniformBuffers(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateDescriptorPool(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateDescriptorSets(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, const std::vector<std::shared_ptr<graphics::CTexture>>& CubeMapList);

		void Release();

		// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateShaderModule(VkShaderModule& shaderModule, const std::string& code);
	public:
		CVulkanMaterial(api::CVulkanAPI* pGraphicsAPI);
		virtual ~CVulkanMaterial();

		virtual bool Create(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, const std::vector<std::shared_ptr<graphics::CTexture>>& CubeMapList) override;
		virtual bool Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) override;
		virtual bool BuildDrawBuffer(int DynamicOffsetNum) override;

		virtual void SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum = -1) override;

		const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages()const { return m_ShaderStages; }
		const VkDescriptorSetLayout& GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
		const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return m_DescriptorSets; }
	};
}
#endif