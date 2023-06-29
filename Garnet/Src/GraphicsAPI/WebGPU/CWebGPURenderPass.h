#pragma once

#ifdef __DAWN__

#include <string>
#include "../ERenderPassFormat.h"

namespace api
{
	class CWebGPURenderPass
	{
		int m_Width;
		int m_Height;
		ERenderPassFormat m_RenderPassFormat;
	public:
		CWebGPURenderPass(const std::string& PassName, int Width, int Height, ERenderPassFormat RenderPassFormat);
		virtual ~CWebGPURenderPass();

		bool Create();
	};
}
#endif