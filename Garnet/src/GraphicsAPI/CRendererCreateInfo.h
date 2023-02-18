#pragma once
#include "../Interface/IRendererCreateInfo.h"

namespace renderer
{
	class CRendererCreateInfo : public IRendererCreateInfo
	{
	public:
		CRendererCreateInfo();
		virtual ~CRendererCreateInfo() = default;
	};
}