#pragma once
#ifdef __DAWN__
#include "../Interface/IRenderer.h"
#include <webgpu.h>
#include <wgpu.h>
#include <vector>
#include <string>

namespace api { class CWebGPUAPI; }

namespace renderer
{
	class CRendererCreateInfo;

	class CWebGPURenderer : public IRenderer
	{
		// API
		api::CWebGPUAPI* m_pGraphicsAPI;

		//
		WGPURenderPipeline m_GraphicsPipeline;
	private:
		// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
		bool CreateGraphicsPipeline(const CRendererCreateInfo& createInfo);

		// Helper Function ///////////////////////////////////////////////////////////////////////
		WGPUShaderModule CreateShaderModule(const std::string& shaderCode);
	public:
		CWebGPURenderer();
		virtual ~CWebGPURenderer();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo) override;
		bool Update() override;
		bool Draw() override;
	};
}
#endif