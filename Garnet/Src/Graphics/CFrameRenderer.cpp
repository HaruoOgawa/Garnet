#include "CFrameRenderer.h"
#include <LoadWorker/CLoadWorker.h>

namespace graphics
{
	// DrawResourceName: フレームテクスチャの参照元, DrawTargetPassName: m_RenderBoardの描画先
	CFrameRenderer::CFrameRenderer(api::IGraphicsAPI* pGraphicsAPI, const std::string& DrawResourceName, const std::string& DrawTargetPassName):
		m_IsLoaded(false),
		m_DrawResourceName(DrawResourceName),
		m_pGraphicsAPI(pGraphicsAPI),
		m_MaterialFrame(std::make_shared<graphics::CMaterialFrame>()),
		m_RenderBoard(std::make_shared<object::C3DObject>(DrawTargetPassName, ""))
	{
	}

	CFrameRenderer::~CFrameRenderer()
	{
	}

	bool CFrameRenderer::Create(resource::CLoadWorker* pLoadWorker, const std::string& FileName)
	{
		pLoadWorker->AddLoadResource(std::make_shared<resource::CMaterialFrameLoader>(FileName, m_MaterialFrame));

		return true;
	}

	bool CFrameRenderer::Load()
	{
		auto RenderTarget = m_pGraphicsAPI->FindOffScreenRenderPass(m_DrawResourceName);
		if (!RenderTarget) return false;

		auto Material = m_MaterialFrame->CreateMaterial(m_pGraphicsAPI, 1, graphics::ECullMode::CULL_NONE);

		for (int Index = 0; Index < static_cast<int>(Material->GetTextureBindingLayoutList().size()); Index++)
		{
			const auto& TexLayout = Material->GetTextureBindingLayoutList()[Index];

			m_RenderBoard->GetTextureSet()->Add2DTexture(RenderTarget->GetFrameTexture(Index));
			Material->ReplaceTextureIndex(TexLayout.TextureName, Index);
		}

		if (!m_RenderBoard->CreatePresetSimply(m_pGraphicsAPI, nullptr, graphics::CPresetPrimitive::CreateBoard(m_pGraphicsAPI), graphics::EPresetPrimitiveType::BOARD,
			Material, nullptr)) return false;

		return true;
	}

	bool CFrameRenderer::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
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

	bool CFrameRenderer::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_RenderBoard->Draw(pGraphicsAPI, false, false, Camera, Projection, DrawInfo)) return false;

		return true;
	}
}