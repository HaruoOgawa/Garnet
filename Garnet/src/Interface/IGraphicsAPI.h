#pragma once
#include <memory>
#include "IRenderer.h"

namespace renderer { class IRenderer; }

namespace api
{
	class IGraphicsAPI
	{
	public:
		virtual std::shared_ptr<renderer::IRenderer> CreateRenderer() = 0;

		virtual bool BeginRender() = 0;
		virtual bool EndRender() = 0;
	};
}