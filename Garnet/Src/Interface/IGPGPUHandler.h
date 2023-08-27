#pragma once

#ifdef USE_GPGPU

#include <glm/glm.hpp>
#include "../Graphics/CMaterial.h"

namespace api
{
	class IGPGPUHandler
	{
	public:
		virtual bool Create() = 0;

		virtual bool Dispatch(const glm::ivec3& GroupCount, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) = 0;

		virtual const std::shared_ptr<graphics::CMaterial>& GetComputeMaterial() = 0;
	};
}
#endif // USE_GPGPU


