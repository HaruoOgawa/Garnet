#include "CFrameRenderer.h"
#include <LoadWorker/CLoadWorker.h>
#include <Message/Console.h>

namespace graphics
{
	// DrawResourceName: フレームテクスチャの参照元, DrawTargetPassName: m_RenderBoardの描画先
	CFrameRenderer::CFrameRenderer(api::IGraphicsAPI* pGraphicsAPI, const std::string& DrawTargetPassName, const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList) :
		m_IsLoaded(false),
		m_pGraphicsAPI(pGraphicsAPI),
		m_MaterialFrame(std::make_shared<graphics::CMaterialFrame>()),
		m_RenderBoard(std::make_shared<object::C3DObject>()),
		m_Material(nullptr),
		m_TextureList(TextureList)
	{
		m_RenderBoard->AddPassName(DrawTargetPassName);
	}

	CFrameRenderer::~CFrameRenderer()
	{
	}

	const std::shared_ptr<CMaterial>& CFrameRenderer::GetMaterial() const
	{
		return m_Material;
	}

	bool CFrameRenderer::Create(resource::CLoadWorker* pLoadWorker, const std::string& FileName)
	{
		pLoadWorker->AddLoadResource(std::make_shared<resource::CMaterialFrameLoader>(FileName, m_MaterialFrame));

		return true;
	}

	bool CFrameRenderer::Load()
	{
		m_Material = m_MaterialFrame->CreateMaterial(m_pGraphicsAPI, graphics::ECullMode::CULL_NONE);

		for (int Index = 0; Index < static_cast<int>(m_Material->GetTextureBindingLayoutList().size()); Index++)
		{
			if (Index >= static_cast<int>(m_TextureList.size()))
			{
				Console::Log("[Error] InValid TextureIndex or TextureList Size - FrameRenderer::Load\n");
				return false;
			}

			const auto& TexLayout = m_Material->GetTextureBindingLayoutList()[Index];

			m_RenderBoard->GetTextureSet()->Add2DTexture(m_TextureList[Index]);
			m_Material->ReplaceTextureIndex(TexLayout.TextureName, Index);
		}

		if (!m_RenderBoard->CreatePresetSimply(m_pGraphicsAPI, nullptr, graphics::CPresetPrimitive::CreateBoard(m_pGraphicsAPI), graphics::EPresetPrimitiveType::BOARD,
			m_Material, nullptr)) return false;

		// 使わないのでリリースしておく
		m_TextureList.clear();
		m_TextureList.shrink_to_fit();

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
		if (!m_RenderBoard->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;

		return true;
	}
}