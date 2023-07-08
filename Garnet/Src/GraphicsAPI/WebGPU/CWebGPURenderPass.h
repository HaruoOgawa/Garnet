#pragma once

#ifdef __DAWN__

#include <webgpu.h>
#include <wgpu.h>

#include <string>

#include "../ERenderPassFormat.h"
#include "../../Interface/IRenderPass.h"

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
		api::ERenderPassFormat m_RenderPassFormat;
		std::shared_ptr<CWebGPUTexture> m_FrameTexture;

		// RenderPass
		WGPURenderPassEncoder m_RenderPass;

		// DepthTexture
		WGPUTexture		m_DepthTexture;
		WGPUTextureView m_DepthTextureView;
	private:
		bool CreateDepthTexture(int Width, int Height);
	public:
		CWebGPURenderPass(api::CWebGPUAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat);
		virtual ~CWebGPURenderPass();

		virtual std::shared_ptr<graphics::CTexture> GetFrameTexture() override;
		WGPURenderPassEncoder GetRenderPass() const { return m_RenderPass; }

		virtual bool Create(int Width, int Height) override;

		virtual bool BeginRenderPass() override;
		virtual bool EndRenderPass() override;
	};
}
#endif