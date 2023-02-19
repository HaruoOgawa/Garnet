#include "CWebGPURenderer.h"
#include "CRendererCreateInfo.h"
#include "../GraphicsAPI/CWebGPUAPI.h"

namespace renderer
{
	CWebGPURenderer::CWebGPURenderer()
	{
	}

	CWebGPURenderer::~CWebGPURenderer()
	{
	}

	bool CWebGPURenderer::Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo)
	{
		auto api = static_cast<api::CWebGPUAPI*>(pGraphicsAPI);

		return true;
	}

	bool CWebGPURenderer::Update()
	{
		return true;
	}

	bool CWebGPURenderer::Draw()
	{
		return true;
	}
}