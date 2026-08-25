#include "CPostProcessSSAO.h"
#include <Graphics/CFrameRenderer.h>
#include <LoadWorker/CLoadWorker.h>

namespace graphics
{
	CPostProcessSSAO::CPostProcessSSAO(const std::string& TargetPassName):
		CValueRegistry("PostProcessSSAORegistry"),
		m_TargetPassName(TargetPassName),
		m_Sharpness(10.0f),
		m_NormalExponent(1.0f),
		m_FilterRadius(0.003f),
		m_KernelRadius(6),
		m_AORadius(0.1),
		m_SSAOMainFrameRenderer(nullptr),
		m_Reduce2x2FrameRenderer(nullptr),
		m_Reduce4x4FrameRenderer(nullptr),
		m_BilateralXBlur4x4FrameRenderer(nullptr),
		m_BilateralYBlur4x4FrameRenderer(nullptr),
		m_UpSampling2x2FrameRenderer(nullptr),
		m_BilateralXBlur2x2FrameRenderer(nullptr),
		m_BilateralYBlur2x2FrameRenderer(nullptr),
		m_UpSamplingOriginFrameRenderer(nullptr),
		m_TemporalAccumulationFrameRenderer(nullptr),
		m_SSAOMixFrameRenderer(nullptr)
	{
	}

	CPostProcessSSAO::~CPostProcessSSAO()
	{
	}

	bool CPostProcessSSAO::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		int TexWidth = 0, TexHeight = 0;
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(2);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSAOMainPass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, -1, -1, State)) return false;

			const auto& Pass = pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOMainPass");
			if (Pass)
			{
				TexWidth = Pass->GetWidth();
				TexHeight = Pass->GetHeight();
			}
		}

		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSAOReduce2x2Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 2, TexHeight / 2, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSAOReduce4x4Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 4, TexHeight / 4, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSAOBilateralXBlur4x4Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 4, TexHeight / 4, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSAOBilateralYBlur4x4Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 4, TexHeight / 4, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSAOUpSampling2x2Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 2, TexHeight / 2, State)) return false;
		}

		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSAOBilateralXBlur2x2Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 2, TexHeight / 2, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSAOBilateralYBlur2x2Pass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth / 2, TexHeight / 2, State)) return false;
		}

		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSAOOriginUpSamplingPass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth, TexHeight, State)) return false;
		}
		
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSAOTemporalPass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth, TexHeight, State)) return false;
		}

		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSAOResultPass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, TexWidth, TexHeight, State)) return false;
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

			m_SSAOMainFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSAOMainPass", TextureList);
			if (!m_SSAOMainFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\GBufferSSAO_MF.json")) return false;
		}

		// GBufferSSAOReduce2x2Pass
		{
			std::vector<std::shared_ptr<CTexture>> TextureList;
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOMainPass")->GetFrameTexture(0));

			m_Reduce2x2FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSAOReduce2x2Pass", TextureList);
			
			// フレームテクスチャのフィルターモードがLINEARになっている前提
			// サイズを小さくしたフレームバッファに描画しただけで、バイリニアフィルタつきのダウンサンプリングの想定
			if (!m_Reduce2x2FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\Downsample_MF.json")) return false;
		}
		
		// GBufferSSAOReduce4x4Pass
		{
			m_Reduce4x4FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSAOReduce4x4Pass", pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOReduce2x2Pass")->GetFrameTextureList());
			
			// フレームテクスチャのフィルターモードがLINEARになっている前提
			// サイズを小さくしたフレームバッファに描画しただけで、バイリニアフィルタつきのダウンサンプリングの想定
			if (!m_Reduce4x4FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\Downsample_MF.json")) return false;
		}
		
		// GBufferSSAOBilateralXBlur4x4Pass
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOReduce4x4Pass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(3));
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(1));

			m_BilateralXBlur4x4FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSAOBilateralXBlur4x4Pass", TextureList);
			if (!m_BilateralXBlur4x4FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\BilateralFilter_MF.json")) return false;
		}
		
		// GBufferSSAOBilateralYBlur4x4Pass
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOBilateralXBlur4x4Pass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(3));
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(1));

			m_BilateralYBlur4x4FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSAOBilateralYBlur4x4Pass", TextureList);
			if (!m_BilateralYBlur4x4FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\BilateralFilter_MF.json")) return false;
		}
		
		// GBufferSSAOUpSampling2x2Pass
		{
			m_UpSampling2x2FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSAOUpSampling2x2Pass", pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOBilateralYBlur4x4Pass")->GetFrameTextureList());
			if (!m_UpSampling2x2FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\UpSampling_MF.json")) return false;
		}
		
		// GBufferSSAOBilateralXBlur2x2Pass
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOUpSampling2x2Pass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(3));
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(1));

			m_BilateralXBlur2x2FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSAOBilateralXBlur2x2Pass", TextureList);
			if (!m_BilateralXBlur2x2FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\BilateralFilter_MF.json")) return false;
		}
		
		// GBufferSSAOBilateralYBlur2x2Pass
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOBilateralXBlur2x2Pass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(3));
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(1));

			m_BilateralYBlur2x2FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSAOBilateralYBlur2x2Pass", TextureList);
			if (!m_BilateralYBlur2x2FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\BilateralFilter_MF.json")) return false;
		}
		
		// GBufferSSAOOriginUpSamplingPass
		{
			m_UpSamplingOriginFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSAOOriginUpSamplingPass", pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOBilateralYBlur2x2Pass")->GetFrameTextureList());
			if (!m_UpSamplingOriginFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\UpSampling_MF.json")) return false;
		}
		
		// GBufferSSAOTemporalPass
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;

			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOOriginUpSamplingPass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOResultPass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass")->GetFrameTexture(6));

			m_TemporalAccumulationFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSAOTemporalPass", TextureList);
			if (!m_TemporalAccumulationFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\TemporalAccumulation_MF.json")) return false;
		}

		// SSAOMix
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

			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOTemporalPass")->GetFrameTexture());
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSAOMainPass")->GetFrameTexture(1));

			m_SSAOMixFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, m_TargetPassName, TextureList);
			if (!m_SSAOMixFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\GBufferSSAOMix_MF.json")) return false;
		}

		return true;
	}

	bool CPostProcessSSAO::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
		const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_SSAOMainFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_Reduce2x2FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_Reduce4x4FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_BilateralXBlur4x4FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_BilateralYBlur4x4FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_UpSampling2x2FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_BilateralXBlur2x2FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_BilateralYBlur2x2FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_UpSamplingOriginFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_TemporalAccumulationFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_SSAOMixFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;

		return true;
	}

	bool CPostProcessSSAO::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera,
		const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		// GBufferSSAOMainPass
		{
			const auto& Material = m_SSAOMainFrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSAOMainPass")) return false;

			if (Material)
			{
				Material->SetUniformValue("aoRadius", &glm::vec1(m_AORadius)[0], sizeof(float));
			}

			if (!m_SSAOMainFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSAOReduce2x2Pass
		{
			if (!pGraphicsAPI->BeginRender("GBufferSSAOReduce2x2Pass")) return false;
			if (!m_Reduce2x2FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSAOReduce4x4Pass
		{
			if (!pGraphicsAPI->BeginRender("GBufferSSAOReduce4x4Pass")) return false;
			if (!m_Reduce4x4FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSAOBilateralXBlur4x4Pass
		{
			const auto& Material = m_BilateralXBlur4x4FrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSAOBilateralXBlur4x4Pass")) return false;
			
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
		
		// GBufferSSAOBilateralYBlur4x4Pass
		{
			const auto& Material = m_BilateralYBlur4x4FrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSAOBilateralYBlur4x4Pass")) return false;

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
		
		// GBufferSSAOUpSampling2x2Pass
		{
			const auto& Material = m_UpSampling2x2FrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSAOUpSampling2x2Pass")) return false;

			if (Material)
			{
				Material->SetUniformValue("filterRadius", &m_FilterRadius, sizeof(float));
			}

			if (!m_UpSampling2x2FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSAOBilateralXBlur2x2Pass
		{
			const auto& Material = m_BilateralXBlur2x2FrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSAOBilateralXBlur2x2Pass")) return false;

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
		
		// GBufferSSAOBilateralYBlur2x2Pass
		{
			const auto& Material = m_BilateralYBlur2x2FrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSAOBilateralYBlur2x2Pass")) return false;

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
		
		// GBufferSSAOOriginUpSamplingPass
		{
			const auto& Material = m_UpSamplingOriginFrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSAOOriginUpSamplingPass")) return false;

			if (Material)
			{
				Material->SetUniformValue("filterRadius", &m_FilterRadius, sizeof(float));
			}

			if (!m_UpSamplingOriginFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// GBufferSSAOTemporalPass
		{
			if (!pGraphicsAPI->BeginRender("GBufferSSAOTemporalPass")) return false;
			if (!m_TemporalAccumulationFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}

		// SSAOの最終結果としてコピーしておく
		if (!pGraphicsAPI->CopyRenderPass("GBufferSSAOTemporalPass", "GBufferSSAOResultPass", true, true)) return false;

		// 最終描画結果にフィードバック
		{
			if (!pGraphicsAPI->BeginRender(m_TargetPassName)) return false;
			if (!m_SSAOMixFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}

		return true;
	}
}