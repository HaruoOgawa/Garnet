#pragma once

#ifndef __DAWN__

#include <string>
#include "../ERenderPassFormat.h"

namespace api
{
	class CVulkanRenderPass
	{
		int m_Width;
		int m_Height;
		ERenderPassFormat m_RenderPassFormat;
	public:
		CVulkanRenderPass(const std::string& PassName, int Width, int Height, ERenderPassFormat RenderPassFormat);
		virtual ~CVulkanRenderPass();

		bool Create();
	};
}
#endif