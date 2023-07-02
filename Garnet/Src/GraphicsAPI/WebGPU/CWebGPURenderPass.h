#pragma once

#ifdef __DAWN__

#include <string>

#include "../ERenderPassFormat.h"
#include "../../Interface/IRenderPass.h"

namespace graphics { class CTexture; }

namespace api
{
	class CWebGPUTexture;

	class CWebGPURenderPass : public graphics::IRenderPass
	{
		// Base Param
		std::string m_PassName;
		int m_Width;
		int m_Height;
		api::ERenderPassFormat m_RenderPassFormat;
		std::shared_ptr<CWebGPUTexture> m_FrameTexture;
	public:
		CWebGPURenderPass(const std::string& PassName, int Width, int Height, ERenderPassFormat RenderPassFormat);
		virtual ~CWebGPURenderPass();

		virtual std::shared_ptr<graphics::CTexture> GetFrameTexture() override;

		bool Create();
	};
}
#endif