#include "CVulkanRenderer.h"
#include "CRendererCreateInfo.h"

namespace renderer
{
	CVulkanRenderer::CVulkanRenderer()
	{
	}

	CVulkanRenderer::~CVulkanRenderer()
	{
	}

	bool CVulkanRenderer::Create(const IRendererCreateInfo& createInfo)
	{
		return true;
	}

	bool CVulkanRenderer::Draw()
	{
		return true;
	}
}