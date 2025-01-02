#pragma once

#ifdef USE_WEBGPU

#include <string>

#include "../ERenderPassFormat.h"
#include "../../Interface/IRenderPass.h"
#include <glm/glm.hpp>
#include "CWebGPUAPI.h"

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
		std::vector<std::shared_ptr<graphics::CTexture>> m_FrameTextureList;
		std::shared_ptr<graphics::CTexture> m_DepthTexture;

		// RenderPass
		WGPURenderPassEncoder m_RenderPass;
	public:
		CWebGPURenderPass(api::CWebGPUAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor);
		virtual ~CWebGPURenderPass();

		virtual std::shared_ptr<graphics::CTexture> GetFrameTexture(int Index = 0) override;
		virtual const std::vector<std::shared_ptr<graphics::CTexture>>& GetFrameTextureList() const override;
		virtual const std::shared_ptr<graphics::CTexture>& GetDepthTexture() const override;
		WGPURenderPassEncoder GetRenderPass() const { return m_RenderPass; }

		virtual bool Create(int Width, int Height, int RenderTargetCount, bool UseColorTexture, bool UseDepthTexture, bool UseStencil) override;

		virtual bool BeginRenderPass() override;
		virtual bool EndRenderPass() override;
	};
}
#endif