#pragma once

#if defined(USE_GPGPU) && defined(USE_WEBGPU)
#include "../../Interface/IGPGPUHandler.h"
#include "CWebGPUAPI.h"

namespace api
{
	class CWebGPUGPGPUHandler : public IGPGPUHandler
	{
		// API
		api::CWebGPUAPI* m_pGraphicsAPI;

		std::shared_ptr<graphics::CMaterial> m_ComputeMaterial;

		// Pipeline
		WGPUPipelineLayout m_ComputePipelineLayout;
		WGPUComputePipeline m_ComputePipeline;

		// Command
		WGPUComputePassEncoder m_ComputePass;
	private:
		bool CreateComputePipeline();

		bool BeginRecordCommandBuffer();
		bool EndRecordCommandBuffer();
	public:
		CWebGPUGPGPUHandler(api::CWebGPUAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterial>& ComputeMaterial);
		virtual ~CWebGPUGPGPUHandler();

		virtual bool Create() override;

		virtual bool Dispatch(const glm::ivec3& GroupCount, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) override;

		virtual const std::shared_ptr<graphics::CMaterial>& GetComputeMaterial() override;
	};
}

#endif // USE_GPGPU