#pragma once
#include "../Interface/IRenderer.h"

namespace api { class CWebGPUAPI; }

namespace renderer
{
	class CRendererCreateInfo;

	class CWebGPURenderer : public IRenderer
	{
	public:
		CWebGPURenderer();
		virtual ~CWebGPURenderer();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo) override;
		bool Update() override;
		bool Draw() override;
	};
}