#ifdef USE_GPGPU
#include "CVulkanGPGPUHandler.h"

namespace graphics
{
	CVulkanGPGPUHandler::CVulkanGPGPUHandler(const std::shared_ptr<graphics::CMaterial>& ComputeMaterial):
		m_ComputeMaterial(ComputeMaterial)
	{
	}

	CVulkanGPGPUHandler::~CVulkanGPGPUHandler()
	{
	}

	bool CVulkanGPGPUHandler::Create()
	{
		if (!m_ComputeMaterial) return false;

		if (!m_ComputeMaterial->Create(std::vector<std::shared_ptr<graphics::CTexture>>(), std::vector<std::shared_ptr<graphics::CTexture>>())) return false;

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
}
#endif // USE_GPGPU