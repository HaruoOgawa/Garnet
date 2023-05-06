#pragma once

#include "../../Interface/IVertex.h"

namespace vertex
{
	class CWebGPUVertex : public IVertex
	{
	public:
		CWebGPUVertex();
		virtual ~CWebGPUVertex();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const CVertexCreateInfo& createInfo) override;
	};
}