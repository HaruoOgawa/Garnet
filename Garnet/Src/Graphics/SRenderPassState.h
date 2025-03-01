#pragma once

namespace graphics
{
	struct SRenderPassState
	{
		int RenderTargetCount = 1;
		bool ColorBuffer = false;
		bool ColorTexture = false;
		bool DepthBuffer = false;
		bool DepthTexture = false;
		bool Stencil = false;
		bool EnabledAA = false; // アンチエイリアス
		int  AASampleNum = 1; // アンチエイリアスのサンプリング数
	};
}