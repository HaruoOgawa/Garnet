#pragma once

#ifdef USE_GPGPU

#include <glm/glm.hpp>
#include "../Graphics/CMaterial.h"

namespace graphics
{
	class IGPGPUHandler
	{
	public:
		virtual bool Create() = 0;

		virtual bool Dispatch(const glm::ivec3& Threads, const glm::ivec3& KernelSize) = 0;

		virtual const std::shared_ptr<graphics::CMaterial>& GetComputeMaterial() = 0;
	};
}
#endif // USE_GPGPU


