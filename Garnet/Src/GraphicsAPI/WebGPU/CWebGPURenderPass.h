#pragma once

#ifdef USE_WEBGPU

#include <webgpu.h>
#include <wgpu.h>

#include <string>

#include "../ERenderPassFormat.h"
#include "../../Interface/IRenderPass.h"
#include <glm/glm.hpp>

namespace graphics { class CTexture; }

namespace api
{
	class CWebGPUAPI;
	class CWebGPUTexture;

	class CWebGPURenderPass : public graphics::IRenderPass
	{
		// API
		api::CWebGPUAPI* m_pGraphicsAPI;

		// Base Param
		std::string m_PassName;
		glm::vec4 m_InitColor;
		api::ERenderPassFormat m_RenderPassFormat;
		std::shared_ptr<CWebGPUTexture> m_FrameTexture;
		std::shared_ptr<CWebGPUTexture> m_DepthTexture;

		// RenderPass
		WGPURenderPassEncoder m_RenderPass;
	public:
		CWebGPURenderPass(api::CWebGPUAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor);
		virtual ~CWebGPURenderPass();

		virtual std::shared_ptr<graphics::CTexture> GetFrameTexture() override;
		virtual std::shared_ptr<graphics::CTexture> GetDepthTexture() override;
		WGPURenderPassEncoder GetRenderPass() const { return m_RenderPass; }

		virtual bool Create(int Width, int Height) override;

		virtual bool BeginRenderPass() override;
		virtual bool EndRenderPass() override;
	};
}
#endif