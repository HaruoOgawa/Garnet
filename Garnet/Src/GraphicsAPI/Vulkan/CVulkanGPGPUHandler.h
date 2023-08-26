#pragma once
#ifdef USE_GPGPU
#include "../../Interface/IGPGPUHandler.h"

namespace graphics
{
	class CVulkanGPGPUHandler : public IGPGPUHandler
	{
		std::shared_ptr<graphics::CMaterial> m_ComputeMaterial;
	public:
		CVulkanGPGPUHandler(const std::shared_ptr<graphics::CMaterial>& ComputeMaterial);
		virtual ~CVulkanGPGPUHandler();

		virtual bool Create() override;

		virtual bool Dispatch(const glm::ivec3& Threads, const glm::ivec3& KernelSize) override;

		virtual const std::shared_ptr<graphics::CMaterial>& GetComputeMaterial() override;
	};
}
#endif // USE_GPGPU