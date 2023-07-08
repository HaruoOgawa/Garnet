#pragma once

#include <memory>

namespace graphics
{
	class CTexture;

	class IRenderPass
	{
	public:
		virtual bool Create(int Width, int Height) = 0;
		
		virtual std::shared_ptr<CTexture> GetFrameTexture() = 0;
		virtual std::shared_ptr<CTexture> GetDepthTexture() = 0;

		virtual bool BeginRenderPass() = 0;
		virtual bool EndRenderPass() = 0;
	};
}