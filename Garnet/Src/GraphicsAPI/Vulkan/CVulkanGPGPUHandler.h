#pragma once
#if defined(USE_GPGPU) && defined(USE_VULKAN)
#include "../../Interface/IGPGPUHandler.h"
#include "CVulkanAPI.h"

namespace api
{
	class CVulkanGPGPUHandler : public IGPGPUHandler
	{
		// API
		api::CVulkanAPI* m_pGraphicsAPI;

		std::shared_ptr<graphics::CMaterial> m_ComputeMaterial;

		// Pipeline
		VkPipelineLayout m_ComputePipelineLayout;
		VkPipeline m_ComputePipeline;

		// Command
		VkCommandPool   m_CommandPool;
		VkCommandBuffer m_CommandBuffer;
	private:
		bool CreateComputePipeline();

		bool BeginRecordCommandBuffer();
		bool EndRecordCommandBuffer();
	public:
		CVulkanGPGPUHandler(api::CVulkanAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterial>& ComputeMaterial);
		virtual ~CVulkanGPGPUHandler();

		virtual bool Create() override;

		virtual bool Dispatch(const glm::ivec3& GroupCount, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) override;

		virtual const std::shared_ptr<graphics::CMaterial>& GetComputeMaterial() override;
	};
}
#endif // USE_GPGPU