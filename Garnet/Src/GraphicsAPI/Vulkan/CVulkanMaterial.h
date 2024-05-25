#pragma once
#ifdef USE_VULKAN

#include <memory>
#include <map>
#include <set>

#include <glfw3.h>
#include <glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../Graphics/CMaterial.h"

namespace graphics { 
	class CMaterialCreateInfo; 
	class CShaderBuffer;
	struct STextureBindingLayout;
}

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
		VkShaderModule m_ComputeShaderModule;

		std::map<VkShaderStageFlagBits, VkShaderEXT> m_ShaderMap;

		// PipelineLayout
		VkPipelineLayout m_PipelineLayout;

		// Uniform Buffer Object
		VkDescriptorSetLayout m_DescriptorSetLayout;

		std::vector<std::vector<VkBuffer>> m_VKUniformBufferList;
		std::vector<std::vector<VkDeviceMemory>> m_VKUniformBufferMemoryList;
		std::vector<std::vector<uint32_t>> m_VKUniformBufferSizeList;

		VkDescriptorPool m_DescriptorPool;
		std::vector<VkDescriptorSet> m_DescriptorSets;
	private:
		// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateShaderObjects(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateShaderStages(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);

		bool CreateDescriptorSetLayout(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateShaderBuffers(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateDescriptorPool(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateDescriptorSets(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);

		bool CreatePipelineLayout();

		void Release();

		// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateShaderModule(VkShaderModule& shaderModule, const std::string& code);
	public:
		CVulkanMaterial(api::CVulkanAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, int RefCount, graphics::ECullMode CullMode);
		virtual ~CVulkanMaterial();

		virtual bool Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet) override;

		virtual bool ReCreate(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::vector<std::shared_ptr<graphics::CShaderBuffer>>& ShaderBufferList, const std::vector<graphics::STextureBindingLayout>& TextureBindingLayoutList) override;

		virtual bool BuildDrawBuffer(int DynamicOffsetNum) override;

		virtual void SetUniformValue(const std::string Name, const void* Data, int ByteSize, int DynamicOffsetNum = -1) override;

		void SetActive();

		void BindUBO(int DynamicOffsetNum);

		const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages()const { return m_ShaderStages; }
		const VkDescriptorSetLayout& GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
		const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return m_DescriptorSets; }
		const std::vector<std::vector<VkBuffer>>& GetVKUniformBufferList() const { return m_VKUniformBufferList; }
		const std::vector<std::vector<uint32_t>>& GetVKUniformBufferSizeList() const { return m_VKUniformBufferSizeList; }
		const VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
	};
}
#endif