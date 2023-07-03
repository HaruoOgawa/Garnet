#ifdef __DAWN__
#include "CWebGPURenderPass.h"
#include "CWebGPUTexture.h"

namespace api
{
	CWebGPURenderPass::CWebGPURenderPass(const std::string& PassName, int Width, int Height, ERenderPassFormat RenderPassFormat):
		m_PassName(PassName),
		m_Width(Width),
		m_Height(Height),
		m_RenderPassFormat(RenderPassFormat),
		m_FrameTexture(nullptr)
	{
	}

	CWebGPURenderPass::~CWebGPURenderPass()
	{
	}

	std::shared_ptr<graphics::CTexture> CWebGPURenderPass::GetFrameTexture()
	{
		return m_FrameTexture;
	}

	bool CWebGPURenderPass::Create()
	{
		return true;
	}
}
#endif
