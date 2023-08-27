#ifdef USE_GPGPU
#include "CVulkanGPGPUHandler.h"
#include "CVulkanMaterial.h"

namespace graphics
{
	CVulkanGPGPUHandler::CVulkanGPGPUHandler(api::CVulkanAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterial>& ComputeMaterial):
		m_pGraphicsAPI(pGraphicsAPI),
		m_ComputeMaterial(ComputeMaterial),
		m_ComputePipelineLayout(nullptr),
		m_ComputePipeline(nullptr)
	{
	}

	CVulkanGPGPUHandler::~CVulkanGPGPUHandler()
	{
	}

	const std::shared_ptr<graphics::CMaterial>& CVulkanGPGPUHandler::GetComputeMaterial()
	{
		return m_ComputeMaterial;
	}

	bool CVulkanGPGPUHandler::Create()
	{
		if (!m_ComputeMaterial) return false;

		if (!m_ComputeMaterial->Create(std::vector<std::shared_ptr<graphics::CTexture>>(), std::vector<std::shared_ptr<graphics::CTexture>>())) return false;
		if (!CreateComputePipeline()) return false; // Compute Pipelineを作成
		if (!m_pGraphicsAPI->CreateCommandPool(m_CommandPool)) return false;
		if (!m_pGraphicsAPI->CreateCommandBuffer(m_CommandBuffer, m_CommandPool)) return false;

		return true;
	}

	bool CVulkanGPGPUHandler::CreateComputePipeline()
	{
		api::CVulkanMaterial* pVulkanMat = static_cast<api::CVulkanMaterial*>(m_ComputeMaterial.get());

		// Layoutの作成
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &pVulkanMat->GetDescriptorSetLayout();

		if (vkCreatePipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_ComputePipelineLayout) != VK_SUCCESS)
		{
			return false;
		}

		// Pipelineの作成
		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = m_ComputePipelineLayout;
		pipelineInfo.stage = pVulkanMat->GetShaderStages()[0];

		if (vkCreateComputePipelines(m_pGraphicsAPI->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_ComputePipeline) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	bool CVulkanGPGPUHandler::Dispatch(const glm::ivec3& GroupCount, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		api::CVulkanMaterial* pVulkanMat = static_cast<api::CVulkanMaterial*>(m_ComputeMaterial.get());

		// データの更新
		if (!pVulkanMat->SetCommonUniform(SecondsTime, Camera, Projection, DrawInfo)) return false;
		if (!pVulkanMat->BuildDrawBuffer(0)) return false;

		// コマンドバッファの記録開始
		if (!BeginRecordCommandBuffer()) return false;

		// Cmd
		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline);
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
			m_ComputePipelineLayout, 0, 1, &pVulkanMat->GetDescriptorSets()[m_pGraphicsAPI->GetCurrentFrame()], 0, nullptr);
		vkCmdDispatch(m_CommandBuffer, GroupCount.x, GroupCount.y, GroupCount.z);

		// コマンドバッファの記録終了
		if (!EndRecordCommandBuffer()) return false;

		return true;
	}

	bool CVulkanGPGPUHandler::BeginRecordCommandBuffer()
	{
		// 前のフレームの処理が終わるのを待つ
		const auto& Fence = m_pGraphicsAPI->GetComputeInFlightFence();
		vkWaitForFences(m_pGraphicsAPI->GetLogicalDevice(), 1, &Fence, VK_TRUE, UINT64_MAX);

		// 処理が終わったのでフェンスをリセットしてまた使える状態にしておく
		vkResetFences(m_pGraphicsAPI->GetLogicalDevice(), 1, &Fence);

		// コマンドバッファをリセットする
		vkResetCommandBuffer(m_CommandBuffer, 0);

		// コマンドバッファの記録開始
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		// Begin
		if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	bool CVulkanGPGPUHandler::EndRecordCommandBuffer()
	{
		// コマンドバッファの記録終了
		if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
		{
			return false;
		}

		// コマンドバッファの送信
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;

		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;

		const auto& Semaphore = m_pGraphicsAPI->GetComputeFlightSemaphore();

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &Semaphore;

		const auto& Fence = m_pGraphicsAPI->GetComputeInFlightFence();

		if (vkQueueSubmit(m_pGraphicsAPI->GetComputeQueue(), 1, &submitInfo, Fence) != VK_SUCCESS) // Compute Queueを実行
		{
			return false;
		}

		return true;
	}
}
#endif // USE_GPGPU