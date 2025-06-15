#if defined(USE_GPGPU) && defined(USE_VULKAN)
#include "CVulkanGPGPUHandler.h"
#include "CVulkanMaterial.h"

namespace api
{
	CVulkanGPGPUHandler::CVulkanGPGPUHandler(api::CVulkanAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterial>& ComputeMaterial):
		m_pGraphicsAPI(pGraphicsAPI),
		m_MinDeltaSecondsTime(1.0f / 60.0f),
		m_ComputeMaterial(ComputeMaterial),

		m_ComputePipelineLayout(nullptr),
		m_ComputePipeline(nullptr),
		m_CommandPool(nullptr),
		m_CommandBuffer(nullptr)
	{
	}

	CVulkanGPGPUHandler::~CVulkanGPGPUHandler()
	{
		if (m_ComputePipelineLayout)
		{
			vkDestroyPipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), m_ComputePipelineLayout, nullptr);
			m_ComputePipelineLayout = nullptr;
		}

		if (m_ComputePipeline)
		{
			vkDestroyPipeline(m_pGraphicsAPI->GetLogicalDevice(), m_ComputePipeline, nullptr);
			m_ComputePipeline = nullptr;
		}

		if (m_CommandPool)
		{
			vkDestroyCommandPool(m_pGraphicsAPI->GetLogicalDevice(), m_CommandPool, nullptr);
			m_CommandPool = nullptr;
		}
	}

	const std::shared_ptr<graphics::CMaterial>& CVulkanGPGPUHandler::GetComputeMaterial()
	{
		return m_ComputeMaterial;
	}

	bool CVulkanGPGPUHandler::Create()
	{
		if (!m_ComputeMaterial) return false;

		if (!m_ComputeMaterial->Create(std::vector<std::string>(), nullptr)) return false;
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

		VK_CHECK_RESULT(vkCreatePipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_ComputePipelineLayout));

		// Pipelineの作成
		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = m_ComputePipelineLayout;
		pipelineInfo.stage = pVulkanMat->GetShaderStages()[0];

		VK_CHECK_RESULT(vkCreateComputePipelines(m_pGraphicsAPI->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_ComputePipeline));

		return true;
	}

	bool CVulkanGPGPUHandler::Dispatch(const glm::ivec3& GroupCount, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		api::CVulkanMaterial* pVulkanMat = static_cast<api::CVulkanMaterial*>(m_ComputeMaterial.get());

		// データの更新
		// DeltaTimeが毎フレーム大きくなったり小さくなったりすると、シミュレーションがガタガタするので0.001000f以上の最小の数を固定のdeltaTimeとする
		const float CurrentDeltaSecondsTime = DrawInfo->GetDeltaSecondsTime();
		m_MinDeltaSecondsTime = glm::max(0.001000f, glm::min(CurrentDeltaSecondsTime, m_MinDeltaSecondsTime));
		DrawInfo->SetDeltaSecondsTime(m_MinDeltaSecondsTime);

		pVulkanMat->SetUniformValue("cameraPos", &Camera->GetPos()[0], sizeof(glm::vec3));
		pVulkanMat->SetUniformValue("time", &glm::vec1(DrawInfo->GetSecondsTime())[0], sizeof(float));
		pVulkanMat->SetUniformValue("deltaTime", &glm::vec1(DrawInfo->GetDeltaSecondsTime())[0], sizeof(float));

		if (!pVulkanMat->BuildDrawBuffer()) return false;

		DrawInfo->SetDeltaSecondsTime(CurrentDeltaSecondsTime); // DeltaTimeを元に戻す

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
		// コマンドバッファをリセットする
		vkResetCommandBuffer(m_CommandBuffer, 0);

		// コマンドバッファの記録開始
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		// Begin
		VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo));

		return true;
	}

	bool CVulkanGPGPUHandler::EndRecordCommandBuffer()
	{
		// コマンドバッファの記録終了
		VK_CHECK_RESULT(vkEndCommandBuffer(m_CommandBuffer));

		const auto& Fence = m_pGraphicsAPI->GetComputeInFlightFence();

		// コマンドバッファの送信
		const auto& Semaphore = m_pGraphicsAPI->GetComputeFlightSemaphore();
		VkSemaphore waitSemaphore[] = { m_pGraphicsAPI->GetRenderFlightSemaphore()}; // セマフォで待つ

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitSemaphores = waitSemaphore;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &Semaphore;

		// Compute Queueを実行
		VK_CHECK_RESULT(vkQueueSubmit(m_pGraphicsAPI->GetComputeQueue(), 1, &submitInfo, Fence));

		// ComutePassが終わるのを待つ
		// レンダリングの方はスワップチェーンパスやオフスクリーンレンダリングパスも含めた全体でvkQueueSubmitやvkWaitForFencesを行うが、
		// ComputePassはレンダリングと同期させる(ComputePassが終わった後にレンダリングを行う)といったことがしたいので
		// 個別にこれらの関数を呼んであげる必要がある
		vkWaitForFences(m_pGraphicsAPI->GetLogicalDevice(), 1, &Fence, VK_TRUE, UINT64_MAX);

		// 処理が終わったのでフェンスをリセットしてまた使える状態にしておく
		vkResetFences(m_pGraphicsAPI->GetLogicalDevice(), 1, &Fence);

		return true;
	}
}
#endif // USE_GPGPU