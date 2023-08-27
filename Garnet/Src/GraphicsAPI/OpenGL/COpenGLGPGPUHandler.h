#pragma once

#if defined(USE_GPGPU) && defined(USE_OPENGL)
#include "../../Interface/IGPGPUHandler.h"
#include "COpenGLAPI.h"

namespace api
{
	class COpenGLGPGPUHandler : public IGPGPUHandler
	{
		// API
		api::COpenGLAPI* m_pGraphicsAPI;

		std::shared_ptr<graphics::CMaterial> m_ComputeMaterial;
	private:
	public:
		COpenGLGPGPUHandler(api::COpenGLAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterial>& ComputeMaterial);
		virtual ~COpenGLGPGPUHandler();

		virtual bool Create() override;

		virtual bool Dispatch(const glm::ivec3& GroupCount, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) override;

		virtual const std::shared_ptr<graphics::CMaterial>& GetComputeMaterial() override;
	};
}
#endif // USE_GPGPU && USE_OPENGL