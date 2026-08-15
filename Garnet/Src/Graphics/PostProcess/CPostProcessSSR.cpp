#include "CPostProcessSSR.h"
#include <Graphics/CFrameRenderer.h>
#include <LoadWorker/CLoadWorker.h>

namespace graphics
{
	CPostProcessSSR::CPostProcessSSR(const std::string& TargetPassName):
		CValueRegistry("PostProcessSSRRegistry"),
		m_TargetPassName(TargetPassName),
		m_MaxDistance(2.0f)
	{
	}

	CPostProcessSSR::~CPostProcessSSR()
	{
	}

	bool CPostProcessSSR::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(2);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSRPass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, -1, -1, State)) return false;
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

			m_SSRMainFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "GBufferSSRPass", TextureList);
			if (!m_SSRMainFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\GBufferSSR_MF.json")) return false;
		}

		// SSRMix
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

			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSRPass")->GetFrameTexture(0));
			TextureList.push_back(pGraphicsAPI->FindOffScreenRenderPass("GBufferSSRPass")->GetFrameTexture(1));

			m_SSRMixFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, m_TargetPassName, TextureList);
			if (!m_SSRMixFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\GBufferSSRMix_MF.json")) return false;
		}

		return true;
	}

	bool CPostProcessSSR::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
		const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_SSRMainFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_SSRMixFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;

		return true;
	}

	bool CPostProcessSSR::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera,
		const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		// GBufferSSRPass
		{
			const auto& Material = m_SSRMainFrameRenderer->GetMaterial();

			if (!pGraphicsAPI->BeginRender("GBufferSSRPass")) return false;

			if (Material)
			{
				Material->SetUniformValue("maxDistance", &glm::vec1(m_MaxDistance)[0], sizeof(float));
				Material->SetUniformValue("near", &glm::vec1(Projection->GetNear())[0], sizeof(float));
				Material->SetUniformValue("far", &glm::vec1(Projection->GetFar())[0], sizeof(float));
			}

			if (!m_SSRMainFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}
		
		// 最終描画結果にフィードバック
		{
			if (!pGraphicsAPI->BeginRender(m_TargetPassName)) return false;
			if (!m_SSRMixFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}

		return true;
	}
}