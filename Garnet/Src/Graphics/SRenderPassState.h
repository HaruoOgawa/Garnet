#pragma once

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
	};
}