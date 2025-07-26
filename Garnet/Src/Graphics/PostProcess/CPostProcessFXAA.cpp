#include "CPostProcessFXAA.h"
#include <Graphics/CFrameRenderer.h>
#include <Message/Console.h>

namespace graphics
{
	CPostProcessFXAA::CPostProcessFXAA(const std::string& TargetPassName) :
		CValueRegistry("PostProcessFXAARegistry"),
		m_TargetPassName(TargetPassName)
	{
	}

	CPostProcessFXAA::~CPostProcessFXAA()
	{
	}

	bool CPostProcessFXAA::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		// FrameBuffer
		if (!pGraphicsAPI->CreateRenderPass("PostProcess_FXAA", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS)) return false;

		// FrameBufferRenderer
		m_FXAAFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "PostProcess_FXAA", pGraphicsAPI->FindOffScreenRenderPass(m_TargetPassName)->GetFrameTextureList());
		if (!m_FXAAFrameRenderer->Create(pLoadWorker, "Resources\\MaterialFrame\\PostProcess_FXAA_MF.json")) return false;

		m_ResultFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, m_TargetPassName, pGraphicsAPI->FindOffScreenRenderPass("PostProcess_FXAA")->GetFrameTextureList());
		if (!m_ResultFrameRenderer->Create(pLoadWorker, "Resources\\MaterialFrame\\FrameTexture_MF.json")) return false;

		return true;
	}

	bool CPostProcessFXAA::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_FXAAFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_ResultFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;

		return true;
	}

	bool CPostProcessFXAA::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		// PostProcess_FXAA
		{
			if (!pGraphicsAPI->BeginRender("PostProcess_FXAA")) return false;
			const auto& Material = m_FXAAFrameRenderer->GetMaterial();
			if (Material)
			{
				const auto& texelSize = 1.0f / Projection->GetScreenResolution();
				Material->SetUniformValue("texelSize", &texelSize[0], sizeof(float) * 2);
			}
			if (!m_FXAAFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}

		// Result Pass
		{
			if (!pGraphicsAPI->BeginRender(m_TargetPassName)) return false;
			if (!m_ResultFrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			if (!pGraphicsAPI->EndRender()) return false;
		}

		return true;
	}
}