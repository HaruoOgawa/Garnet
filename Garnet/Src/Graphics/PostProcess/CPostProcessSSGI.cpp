#include "CPostProcessSSGI.h"
#include <Graphics/CFrameRenderer.h>

namespace graphics
{
	CPostProcessSSGI::CPostProcessSSGI(const std::string& TargetPassName):
		CValueRegistry("PostProcessSSGIRegistry"),
		m_TargetPassName(TargetPassName),
		m_SSGIMainFrameRenderer(nullptr),
		m_Reduce2x2FrameRenderer(nullptr),
		m_Reduce4x4FrameRenderer(nullptr)
	{
	}

	CPostProcessSSGI::~CPostProcessSSGI()
	{
	}

	bool CPostProcessSSGI::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		{
			graphics::SRenderPassState State = graphics::SRenderPassState(1);
			if (!pGraphicsAPI->CreateRenderPass("GBufferSSGIMainPass", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS, -1, -1, State)) return false;
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

		return true;
	}

	bool CPostProcessSSGI::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
		const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_SSGIMainFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;

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

		return true;
	}
}