#pragma once

#include <vector>
#include <memory>

namespace graphics
{
	class CTexture;

	class IRenderPass
	{
	public:
		virtual bool Create(int Width, int Height, int RenderTargetCount, bool UseColorTexture, bool UseDepthTexture, bool UseStencil) = 0;
		
		virtual std::shared_ptr<CTexture> GetFrameTexture(int Index = 0) = 0;
		virtual const std::vector<std::shared_ptr<CTexture>>& GetFrameTextureList() const = 0;
		virtual const std::shared_ptr<CTexture>& GetDepthTexture() const = 0;

		virtual bool BeginRenderPass() = 0;
		virtual bool EndRenderPass() = 0;
	};
}