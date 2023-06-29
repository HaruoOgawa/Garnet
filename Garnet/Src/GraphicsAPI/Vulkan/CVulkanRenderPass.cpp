#ifndef __DAWN__
#include "CVulkanRenderPass.h"

namespace api
{
	CVulkanRenderPass::CVulkanRenderPass(const std::string& PassName, int Width, int Height, ERenderPassFormat RenderPassFormat):
		m_Width(Width),
		m_Height(Height),
		m_RenderPassFormat(RenderPassFormat)
	{
	}

	CVulkanRenderPass::~CVulkanRenderPass()
	{
	}

	bool CVulkanRenderPass::Create()
	{
		return true;
	}
}
#endif
