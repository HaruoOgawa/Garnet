#include "CDeferredRenderer.h"
#include <LoadWorker/CLoadWorker.h>

namespace graphics
{
	CDeferredRenderer::CDeferredRenderer(api::IGraphicsAPI* pGraphicsAPI):
		m_IsLoaded(false),
		m_pGraphicsAPI(pGraphicsAPI),
		m_MRTMF(std::make_shared<graphics::CMaterialFrame>()),
		m_RenderBoard(std::make_shared<object::C3DObject>("", ""))
	{
	}

	CDeferredRenderer::~CDeferredRenderer()
	{
	}

	bool CDeferredRenderer::Create(resource::CLoadWorker* pLoadWorker)
	{
		pLoadWorker->AddLoadResource(std::make_shared<resource::CMaterialFrameLoader>("Resources\\MaterialFrame\\MRTSample_MF.json", m_MRTMF));

		return true;
	}

	bool CDeferredRenderer::Load()
	{
		auto RenderTarget = m_pGraphicsAPI->FindOffScreenRenderPass("MRTTest");
		if (!RenderTarget) return false;

		m_RenderBoard->GetTextureSet()->Add2DTexture(RenderTarget->GetFrameTexture(0));
		m_RenderBoard->GetTextureSet()->Add2DTexture(RenderTarget->GetFrameTexture(1));
		m_RenderBoard->GetTextureSet()->Add2DTexture(RenderTarget->GetFrameTexture(2));

		auto Material = m_MRTMF->CreateMaterial(m_pGraphicsAPI, 1, graphics::ECullMode::CULL_BACK);
		Material->ReplaceTextureIndex("texGPosition", 0);
		Material->ReplaceTextureIndex("texGNormal", 1);
		Material->ReplaceTextureIndex("texGAlbedo", 2);

		if (!m_RenderBoard->CreatePresetSimply(m_pGraphicsAPI, nullptr, graphics::CPresetPrimitive::CreateBoard(m_pGraphicsAPI), graphics::EPresetPrimitiveType::BOARD,
			Material, nullptr)) return false;

		return true;
	}

	bool CDeferredRenderer::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_IsLoaded)
		{
			if (!pLoadWorker->IsLoaded()) return true;

			if (!Load()) return false;

			m_IsLoaded = true;
		}

		if (!m_RenderBoard->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;

		return true;
	}

	bool CDeferredRenderer::Draw(const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_RenderBoard->Draw(false, false, Camera, Projection, DrawInfo)) return false;

		return true;
	}
}