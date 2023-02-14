#pragma once
#include <memory>
#include "IRenderer.h"

namespace renderer { class IRenderer; }

namespace api
{
	class IGraphicsAPI
	{
	public:
		virtual bool Initialize() = 0;
		virtual void Release() = 0;
		virtual std::shared_ptr<renderer::IRenderer> CreateRenderer() = 0;
	};
}