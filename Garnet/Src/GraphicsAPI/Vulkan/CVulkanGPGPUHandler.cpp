#ifdef USE_GPGPU
#include "CVulkanGPGPUHandler.h"
#include "CVulkanMaterial.h"

namespace graphics
{
	CVulkanGPGPUHandler::CVulkanGPGPUHandler(api::CVulkanAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterial>& ComputeMaterial):
		m_pGraphicsAPI(pGraphicsAPI),
		m_ComputeMaterial(ComputeMaterial),
		m_ComputePipeline(nullptr)
	{
	}

	CVulkanGPGPUHandler::~CVulkanGPGPUHandler()
	{
	}

	bool CVulkanGPGPUHandler::Create()
	{
		if (!m_ComputeMaterial) return false;

		if (!m_ComputeMaterial->Create(std::vector<std::shared_ptr<graphics::CTexture>>(), std::vector<std::shared_ptr<graphics::CTexture>>())) return false;
		if (!CreateComputePipeline()) return false; // Compute PipelineÇçÏê¨

		return true;
	}

	bool CVulkanGPGPUHandler::Dispatch(const glm::ivec3& Threads, const glm::ivec3& KernelSize)
	{
		return true;
	}

	const std::shared_ptr<graphics::CMaterial>& CVulkanGPGPUHandler::GetComputeMaterial()
	{
		return m_ComputeMaterial;
	}

	bool CVulkanGPGPUHandler::CreateComputePipeline()
	{
		api::CVulkanMaterial* pVulkanMat = static_cast<api::CVulkanMaterial*>(m_ComputeMaterial.get());

		// LayoutÇÃçÏê¨
		VkPipelineLayout computePipelineLayout{};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &pVulkanMat->GetDescriptorSetLayout();

		if (vkCreatePipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &computePipelineLayout) != VK_SUCCESS)
		{
			return false;
		}

		// PipelineÇÃçÏê¨
		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = computePipelineLayout;
		pipelineInfo.stage = pVulkanMat->GetShaderStages()[0];

		if (vkCreateComputePipelines(m_pGraphicsAPI->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_ComputePipeline) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}
}
#endif // USE_GPGPU