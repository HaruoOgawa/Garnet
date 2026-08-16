#include "CPostProcessSSGI.h"
#include <Graphics/CFrameRenderer.h>
#include <LoadWorker/CLoadWorker.h>

namespace graphics
{
	CPostProcessSSGI::CPostProcessSSGI(const std::string& TargetPassName):
		CValueRegistry("PostProcessSSGIRegistry"),
		m_TargetPassName(TargetPassName),
		m_Sharpness(1.0f),
		m_NormalExponent(1.0f),
		m_FilterRadius(0.003f),
		m_KernelRadius(6),
		m_MaxDistance(2.0f),
		m_SSGIMainFrameRenderer(nullptr),
		m_Reduce2x2FrameRenderer(nullptr),
		m_Reduce4x4FrameRenderer(nullptr),
		m_BilateralXBlur4x4FrameRenderer(nullptr),
		m_BilateralYBlur4x4FrameRenderer(nullptr),
		m_UpSampling2x2FrameRenderer(nullptr),
		m_BilateralXBlur2x2FrameRenderer(nullptr),
		m_BilateralYBlur2x2FrameRenderer(nullptr),
		m_UpSamplingOriginFrameRenderer(nullptr),
		m_TemporalAccumulationFrameRenderer(nullptr),
		m_SSGIMixFrameRenderer(nullptr)
	{
	}

	CPostProcessSSGI::~CPostProcessSSGI()
	{
	}

	bool CPostProcessSSGI::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		int TexWidth = 0, TexHeight = 0;
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(2);
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
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIBilateralXBlur4x4Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 4, TexHeight / 4, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIBilateralYBlur4x4Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 4, TexHeight / 4, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIUpSampling2x2Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 2, TexHeight / 2, State)) return false;
		}

		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIBilateralXBlur2x2Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 2, TexHeight / 2, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIBilateralYBlur2x2Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 2, TexHeight / 2, State)) return false;
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

			auto TargetPass = pGraphicsAPI->FindOffScreenRenderPass(m_TargetPassName);
			if (TargetPass)
			{
				TextureList.push_back(TargetPass->GetFrameTexture());
			}

			auto MainResultPass = pGraphicsAPI->FindOffScreenRenderPass("MainResultPass");
			if (MainResultPass)
			{
				TextureList.push_back(MainResultPass->GetFrameTexture());
			}

			m_SSGIMainFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIMainPass", TextureList);
			if (!m_SSGIMainFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\GBufferSSGIMain_MF.json")) return false;
		}

		// GBufferSSGIReduce2x2Pass
		{
			std::vector<std::shared_ptr<CTexture>> TextureList;
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIMainPass")->GetFrameTexture(0));

			m_Reduce2x2FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIReduce2x2Pass", TextureList);
			
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
		
		// GBufferSSGIBilateralXBlur4x4Pass
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIReduce4x4Pass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(3));
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(1));

			m_BilateralXBlur4x4FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIBilateralXBlur4x4Pass", TextureList);
			if (!m_BilateralXBlur4x4FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\BilateralFilter_MF.json")) return false;
		}
		
		// GBufferSSGIBilateralYBlur4x4Pass
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIBilateralXBlur4x4Pass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(3));
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(1));

			m_BilateralYBlur4x4FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIBilateralYBlur4x4Pass", TextureList);
			if (!m_BilateralYBlur4x4FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\BilateralFilter_MF.json")) return false;
		}
		
		// GBufferSSGIUpSampling2x2Pass
		{
			m_UpSampling2x2FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIUpSampling2x2Pass", pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIBilateralYBlur4x4Pass")->GetFrameTextureList());
			if (!m_UpSampling2x2FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\UpSampling_MF.json")) return false;
		}
		
		// GBufferSSGIBilateralXBlur2x2Pass
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIUpSampling2x2Pass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(3));
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(1));

			m_BilateralXBlur2x2FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIBilateralXBlur2x2Pass", TextureList);
			if (!m_BilateralXBlur2x2FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\BilateralFilter_MF.json")) return false;
		}
		
		// GBufferSSGIBilateralYBlur2x2Pass
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIBilateralXBlur2x2Pass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(3));
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(1));

			m_BilateralYBlur2x2FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIBilateralYBlur2x2Pass", TextureList);
			if (!m_BilateralYBlur2x2FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\BilateralFilter_MF.json")) return false;
		}
		
		// GBufferSSGIOriginUpSamplingPass
		{
			m_UpSamplingOriginFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGIOriginUpSamplingPass", pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIBilateralYBlur2x2Pass")->GetFrameTextureList());
			if (!m_UpSamplingOriginFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\UpSampling_MF.json")) return false;
		}
		
		// GBufferSSGITemporalPass
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;

			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIOriginUpSamplingPass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIResultPass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(6));

			m_TemporalAccumulationFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSGITemporalPass", TextureList);
			if (!m_TemporalAccumulationFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\TemporalAccumulation_MF.json")) return false;
		}

		// SSGIMix
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

			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGITemporalPass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSGIMainPass")->GetFrameTexture(1));

			m_SSGIMixFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, m_TargetPassName, TextureList);
			if (!m_SSGIMixFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\GBufferSSGIMix_MF.json")) return false;
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
		if (!m_BilateralXBlur4x4FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_BilateralYBlur4x4FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_UpSampling2x2FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_BilateralXBlur2x2FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_BilateralYBlur2x2FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_UpSamplingOriginFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_TemporalAccumulationFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_SSGIMixFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;

		return true;
	}

	bool CPostProcessSSGI::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera,
		const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		// GBufferSSGIMainPass
		{
			const auto& Material = m_SSGIMainFrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSGIMainPass")) return false;

			if (Material)
			{
				Material->SetUniformValue("maxDistance", &glm::vec1(m_MaxDistance)[0], sizeof(float));
			}

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
		
		// GBufferSSGIBilateralXBlur4x4Pass
		{
			const auto& Material = m_BilateralXBlur4x4FrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSGIBilateralXBlur4x4Pass")) return false;
			
			if (Material)
			{
				Material->SetUniformValue("g_InvResolutionDirection", &glm::vec2(1.0f, 0.0f)[0], sizeof(float) * 2); // X方向ブラー
				Material->SetUniformValue("g_Sharpness", &glm::vec1(m_Sharpness)[0], sizeof(float));
				Material->SetUniformValue("nExponent", &glm::vec1(m_NormalExponent)[0], sizeof(float));
				Material->SetUniformValue("kernelRadius", &m_KernelRadius, sizeof(int));
			}
			
			if (!m_BilateralXBlur4x4FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGIBilateralYBlur4x4Pass
		{
			const auto& Material = m_BilateralYBlur4x4FrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSGIBilateralYBlur4x4Pass")) return false;

			if (Material)
			{
				Material->SetUniformValue("g_InvResolutionDirection", &glm::vec2(0.0f, 1.0f)[0], sizeof(float) * 2); // Y方向ブラー
				Material->SetUniformValue("g_Sharpness", &glm::vec1(m_Sharpness)[0], sizeof(float));
				Material->SetUniformValue("nExponent", &glm::vec1(m_NormalExponent)[0], sizeof(float));
				Material->SetUniformValue("kernelRadius", &m_KernelRadius, sizeof(int));
			}

			if (!m_BilateralYBlur4x4FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGIUpSampling2x2Pass
		{
			const auto& Material = m_UpSampling2x2FrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSGIUpSampling2x2Pass")) return false;

			if (Material)
			{
				Material->SetUniformValue("filterRadius", &m_FilterRadius, sizeof(float));
			}

			if (!m_UpSampling2x2FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGIBilateralXBlur2x2Pass
		{
			const auto& Material = m_BilateralXBlur2x2FrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSGIBilateralXBlur2x2Pass")) return false;

			if (Material)
			{
				Material->SetUniformValue("g_InvResolutionDirection", &glm::vec2(1.0f, 0.0f)[0], sizeof(float) * 2); // X方向ブラー
				Material->SetUniformValue("g_Sharpness", &glm::vec1(m_Sharpness)[0], sizeof(float));
				Material->SetUniformValue("nExponent", &glm::vec1(m_NormalExponent)[0], sizeof(float));
				Material->SetUniformValue("kernelRadius", &m_KernelRadius, sizeof(int));
			}
			
			if (!m_BilateralXBlur2x2FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGIBilateralYBlur2x2Pass
		{
			const auto& Material = m_BilateralYBlur2x2FrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSGIBilateralYBlur2x2Pass")) return false;

			if (Material)
			{
				Material->SetUniformValue("g_InvResolutionDirection", &glm::vec2(0.0f, 1.0f)[0], sizeof(float) * 2); // Y方向ブラー
				Material->SetUniformValue("g_Sharpness", &glm::vec1(m_Sharpness)[0], sizeof(float));
				Material->SetUniformValue("nExponent", &glm::vec1(m_NormalExponent)[0], sizeof(float));
				Material->SetUniformValue("kernelRadius", &m_KernelRadius, sizeof(int));
			}
			
			if (!m_BilateralYBlur2x2FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGIOriginUpSamplingPass
		{
			const auto& Material = m_UpSamplingOriginFrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSGIOriginUpSamplingPass")) return false;

			if (Material)
			{
				Material->SetUniformValue("filterRadius", &m_FilterRadius, sizeof(float));
			}

			if (!m_UpSamplingOriginFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSGITemporalPass
		{
			if (!pGraphicsAPI->BeginRender("GBufferSSGITemporalPass")) return false;
			if (!m_TemporalAccumulationFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}

		// SSGIの最終結果としてコピーしておく
		if (!pGraphicsAPI->CopyRenderPass("GBufferSSGITemporalPass", "GBufferSSGIResultPass", true, true)) return false;

		// 最終描画結果にフィードバック
		{
			if (!pGraphicsAPI->BeginRender(m_TargetPassName)) return false;
			if (!m_SSGIMixFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}

		return true;
	}
}