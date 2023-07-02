#pragma once

#include <memory>

namespace graphics
{
	class CTexture;

	class IRenderPass
	{
	public:
		virtual std::shared_ptr<CTexture> GetFrameTexture() = 0;
	};
}