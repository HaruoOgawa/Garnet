#pragma once
#include "IRendererCreateInfo.h"

namespace renderer
{
	class IRenderer
	{
		virtual bool Create(const IRendererCreateInfo& createInfo) = 0;
		virtual bool Draw() = 0;
	};
}