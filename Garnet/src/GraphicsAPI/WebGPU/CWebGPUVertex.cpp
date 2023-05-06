#include "CWebGPUVertex.h"
#include "../CVertexCreateInfo.h"

namespace vertex
{
	CWebGPUVertex::CWebGPUVertex()
	{

	}

	CWebGPUVertex::~CWebGPUVertex()
	{
	}

	bool CWebGPUVertex::Create(api::IGraphicsAPI* pGraphicsAPI, const CVertexCreateInfo& createInfo)
	{
		return true;
	}
}