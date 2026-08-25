#include "CPostProcessToneMapping.h"
#include <Graphics/CFrameRenderer.h>
#include <Message/Console.h>

namespace graphics
{
	CPostProcessToneMapping::CPostProcessToneMapping(const std::string& TargetPassName) :
		CValueRegistry("PostProcessToneMappingRegistry"),
		m_TargetPassName(TargetPassName),
		m_TargetTexture(nullptr)
	{
	}

	CPostProcessToneMapping::~CPostProcessToneMapping()
	{
	}

	bool CPostProcessToneMapping::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		// FrameBuffer
		if (!pGraphicsAPI->CreateRenderPass("PostProcess_ToneMapping", api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS)) return false;

		const auto& TargetTextureList = pGraphicsAPI->FindOffScreenRenderPass(m_TargetPassName)->GetFrameTextureList();
		if (!TargetTextureList.empty()) m_TargetTexture = TargetTextureList[0];
		if (!m_TargetTexture) return false;

		// FrameBufferRenderer
		m_FrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, "PostProcess_ToneMapping", TargetTextureList);
		if (!m_FrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\PostProcess_ToneMapping_MF.json")) return false;

		m_ResultFrameRenderer = std::make_shared<graphics::CFrameRenderer>(pGraphicsAPI, m_TargetPassName, pGraphicsAPI->FindOffScreenRenderPass("PostProcess_ToneMapping")->GetFrameTextureList());
		if (!m_ResultFrameRenderer->Create(pLoadWorker, "Resources\\Common\\MaterialFrame\\FrameTexture_MF.json")) return false;

		return true;
	}

	bool CPostProcessToneMapping::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_FrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_ResultFrameRenderer->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;

		return true;
	}

	bool CPostProcessToneMapping::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		// PostProcess_ToneMapping
		{
			if (!pGraphicsAPI->BeginRender("PostProcess_ToneMapping")) return false;
			if (!m_FrameRenderer->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
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