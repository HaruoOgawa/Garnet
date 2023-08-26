#pragma once
#ifdef USE_GPGPU
#include "../../Interface/IGPGPUHandler.h"
#include "CVulkanAPI.h"

namespace graphics
{
	class CVulkanGPGPUHandler : public IGPGPUHandler
	{
		// API
		api::CVulkanAPI* m_pGraphicsAPI;

		std::shared_ptr<graphics::CMaterial> m_ComputeMaterial;

		VkPipeline m_ComputePipeline;
	private:
		bool CreateComputePipeline();
	public:
		CVulkanGPGPUHandler(api::CVulkanAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterial>& ComputeMaterial);
		virtual ~CVulkanGPGPUHandler();

		virtual bool Create() override;

		virtual bool Dispatch(const glm::ivec3& Threads, const glm::ivec3& KernelSize) override;

		virtual const std::shared_ptr<graphics::CMaterial>& GetComputeMaterial() override;
	};
}
#endif // USE_GPGPU