#include "CPostProcessSSGI.h"
#include <Graphics/CFrameRenderer.h>

namespace graphics
{
	CPostProcessSSGI::CPostProcessSSGI(const std::string& TargetPassName):
		CValueRegistry("PostProcessSSGIRegistry"),
		m_TargetPassName(TargetPassName),
		m_SSGIMainFrameRenderer(nullptr),
		m_Reduce2x2FrameRenderer(nullptr),
		m_Reduce4x4FrameRenderer(nullptr),
		m_BilateralBlur4x4FrameRenderer(nullptr),
		m_UpSampling2x2FrameRenderer(nullptr),
		m_BilateralBlur2x2FrameRenderer(nullptr),
		m_UpSamplingOriginFrameRenderer(nullptr),
		m_TemporalFrameRenderer(nullptr)
	{
	}

	CPostProcessSSGI::~CPostProcessSSGI()
	{
	}

	bool CPostProcessSSGI::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		int TexWidth = 0, TexHeight = 0;
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIMainPass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, -1, -1, State)) return false;

			const auto& Pass = pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIMainPass");
			if (Pass)
			{
				TexWidth = Pass->GetWidth();
				TexHeight = Pass->GetHeight();
			}
		}

		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIReduce2x2Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 2, TexHeight / 2, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIReduce4x4Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 4, TexHeight / 4, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIBilateralFilter4x4Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 4, TexHeight / 4, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIUpSampling2x2Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 2, TexHeight / 2, State)) return false;
		}

		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIBilateralFilter2x2Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 2, TexHeight / 2, State)) return false;
		}

		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIOriginUpSamplingPass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth, TexHeight, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGITemporalPass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth, TexHeight, State)) return false;
		}

		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIResultPass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth, TexHeight, State)) return false;
		}

		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;

			auto GBufferGenPass = pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass");
			if (GBufferGenPass)
			{
				for (const auto& Texture : GBufferGenPass->GetFrameTextureList())
				{
					TextureList.push_back(Texture);
				}
			}

			auto GBufferResultPass = pGraphicsAPI->FindOffScreenRenderPass(m_TargetPassName);
			if (GBufferResultPass)
			{
				TextureList.push_back(GBufferResultPass->GetFrameTexture());
			}

			m_SSGIMainFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIMainPass", TextureList);
			if (!m_SSGIMainFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\GBufferSSGIMain_MF.json")) return false;
		}

		// GBufferSSGIReduce2x2Pass
		{
			m_Reduce2x2FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIReduce2x2Pass", pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIMainPass")->GetFrameTextureList());
			
			// フレームテクスチャのフィルターモードがLINEARになっている前提
			// サイズを小さくしたフレームバッファに描画しただけで、バイリニアフィルタつきのダウンサンプリングの想定
			if (!m_Reduce2x2FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\FrameTexture_MF.json")) return false;
		}
		
		// GBufferSSGIReduce4x4Pass
		{
			m_Reduce4x4FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIReduce4x4Pass", pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIReduce2x2Pass")->GetFrameTextureList());
			
			// フレームテクスチャのフィルターモードがLINEARになっている前提
			// サイズを小さくしたフレームバッファに描画しただけで、バイリニアフィルタつきのダウンサンプリングの想定
			if (!m_Reduce4x4FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\FrameTexture_MF.json")) return false;
		}
		
		// GBufferSSGIBilateralFilter4x4Pass
		{
			m_BilateralBlur4x4FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIBilateralFilter4x4Pass", pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIReduce4x4Pass")->GetFrameTextureList());
			if (!m_BilateralBlur4x4FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\BilateralFilter_MF.json")) return false;
		}
		
		// GBufferSSGIUpSampling2x2Pass
		{
			m_UpSampling2x2FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIUpSampling2x2Pass", pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIBilateralFilter4x4Pass")->GetFrameTextureList());
			if (!m_UpSampling2x2FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\UpSampling_MF.json")) return false;
		}
		
		// GBufferSSGIBilateralFilter2x2Pass
		{
			m_BilateralBlur2x2FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIBilateralFilter2x2Pass", pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIUpSampling2x2Pass")->GetFrameTextureList());
			if (!m_BilateralBlur2x2FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\BilateralFilter_MF.json")) return false;
		}
		
		// GBufferSSGIOriginUpSamplingPass
		{
			m_UpSamplingOriginFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIOriginUpSamplingPass", pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIBilateralFilter2x2Pass")->GetFrameTextureList());
			if (!m_UpSamplingOriginFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\UpSampling_MF.json")) return false;
		}
		
		// GBufferSSGITemporalPass
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;

			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIOriginUpSamplingPass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIResultPass")->GetFrameTexture());

			m_TemporalFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGITemporalPass", TextureList);
			if (!m_TemporalFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\GBufferSSGITemporal_MF.json")) return false;
		}

		return true;
	}

	bool CPostProcessSSGI::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
		const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_SSGIMainFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_Reduce2x2FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_Reduce4x4FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_BilateralBlur4x4FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_UpSampling2x2FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_BilateralBlur2x2FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_UpSamplingOriginFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_TemporalFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;

		return true;
	}

	bool CPostProcessSSGI::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera,
		const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		// GBufferSSGIMainPass
		{
			if (!pGraphicsAPI->BeginRender("GBufferSSGIMainPass")) return false;
			if (!m_SSGIMainFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGIReduce2x2Pass
		{
			if (!pGraphicsAPI->BeginRender("GBufferSSGIReduce2x2Pass")) return false;
			if (!m_Reduce2x2FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGIReduce4x4Pass
		{
			if (!pGraphicsAPI->BeginRender("GBufferSSGIReduce4x4Pass")) return false;
			if (!m_Reduce4x4FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGIBilateralFilter4x4Pass
		{
			if (!pGraphicsAPI->BeginRender("GBufferSSGIBilateralFilter4x4Pass")) return false;
			if (!m_BilateralBlur4x4FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGIUpSampling2x2Pass
		{
			if (!pGraphicsAPI->BeginRender("GBufferSSGIUpSampling2x2Pass")) return false;
			if (!m_UpSampling2x2FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGIBilateralFilter2x2Pass
		{
			if (!pGraphicsAPI->BeginRender("GBufferSSGIBilateralFilter2x2Pass")) return false;
			if (!m_BilateralBlur2x2FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGIOriginUpSamplingPass
		{
			if (!pGraphicsAPI->BeginRender("GBufferSSGIOriginUpSamplingPass")) return false;
			if (!m_UpSamplingOriginFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGITemporalPass
		{
			if (!pGraphicsAPI->BeginRender("GBufferSSGITemporalPass")) return false;
			if (!m_TemporalFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}

		// SSGIの最終結果としてコピーしておく
		if (!pGraphicsAPI->CopyRenderPass("GBufferSSGITemporalPass", "GBufferSSGIResultPass", true, true)) return false;

		// 最終描画結果にフィードバック

		return true;
	}
}