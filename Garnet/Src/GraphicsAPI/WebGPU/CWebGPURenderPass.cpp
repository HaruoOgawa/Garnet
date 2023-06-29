#ifdef __DAWN__
#include "CWebGPURenderPass.h"

namespace api
{
	CWebGPURenderPass::CWebGPURenderPass(const std::string& PassName, int Width, int Height, ERenderPassFormat RenderPassFormat):
		m_Width(Width),
		m_Height(Height),
		m_RenderPassFormat(RenderPassFormat)
	{
	}

	CWebGPURenderPass::~CWebGPURenderPass()
	{
	}

	bool CWebGPURenderPass::Create()
	{
		return true;
	}
}
#endif
