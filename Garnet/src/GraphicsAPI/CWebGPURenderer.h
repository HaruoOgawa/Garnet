#pragma once
#include "../Interface/IRenderer.h"

namespace renderer
{
	class CWebGPURenderer : public IRenderer
	{
	public:
		CWebGPURenderer();
		virtual ~CWebGPURenderer();

		bool Create(const IRendererCreateInfo& createInfo) override;
		bool Draw() override;
	};
}