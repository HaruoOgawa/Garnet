#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace graphics
{
	struct SRenderPassState
	{
		int RenderTargetCount = 1;
		bool ColorBuffer = true;
		bool ColorTexture = true;
		bool DepthBuffer = true;
		bool DepthTexture = false;
		bool Stencil = false;
		bool EnabledAA = false; // アンチエイリアス
		int  AASampleNum = 1; // アンチエイリアスのサンプリング数
		std::vector<glm::vec4> InitColorList;

	public:
		SRenderPassState(int _RenderTargetCount = 1, const glm::vec4& _CommonInitColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)):
			RenderTargetCount(_RenderTargetCount)
		{
			InitColorList.resize(RenderTargetCount, _CommonInitColor);
		}
	};
}