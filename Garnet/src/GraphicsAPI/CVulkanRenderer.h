#pragma once
#include "../Interface/IRenderer.h"

namespace renderer
{
	class CVulkanRenderer : public IRenderer
	{
	public:
		CVulkanRenderer();
		virtual ~CVulkanRenderer();

		bool Create(const IRendererCreateInfo& createInfo) override;
		bool Draw() override;
	};
}