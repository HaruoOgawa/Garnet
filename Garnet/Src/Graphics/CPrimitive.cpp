#include "CPrimitive.h"
#include "CMaterial.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"

namespace graphics
{
	CPrimitive::CPrimitive(const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo, int MaterialIndex,
		EPresetPrimitiveType PresetType) :
		m_Renderer(nullptr),
		m_CreateInfo(createInfo),
		m_PresetType(PresetType),
		m_MaterialIndex(MaterialIndex)
	{
	}
	
	CPrimitive::~CPrimitive()
	{
	}

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterial>& Material)
	{
		if (m_CreateInfo)
		{
			if (!Create(pGraphicsAPI, Material, m_CreateInfo)) return false;
		}
		else
		{
			if (!Create(pGraphicsAPI, Material, m_PresetType)) return false;
		}

		// 生成処理が終わったので不要なリソースを解放する
		m_CreateInfo = nullptr;

		return true;
	}

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CMaterial>& Material, const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo)
	{
		m_Renderer = pGraphicsAPI->CreateRenderer();
		if (!m_Renderer->Create(createInfo, Material)) return false;

		return true;
	}

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CMaterial>& Material, EPresetPrimitiveType PresetType)
	{
		std::shared_ptr<renderer::CRendererCreateInfo> createInfo = std::make_shared<renderer::CRendererCreateInfo>();

		switch (PresetType)
		{
		case graphics::EPresetPrimitiveType::BOARD:
			if (!CPresetPrimitive::CreateBoard(createInfo)) return false;
			break;
		case graphics::EPresetPrimitiveType::BOX:
			break;
		case graphics::EPresetPrimitiveType::POINT:
			break;
		case graphics::EPresetPrimitiveType::SPHERE:
			break;
		default:
			break;
		}

		m_Renderer = pGraphicsAPI->CreateRenderer();
		if (!m_Renderer->Create(createInfo, Material)) return false;

		return true;
	}

	bool CPrimitive::Draw(const std::shared_ptr<CMaterial>& Material, int DynamicOffsetNum)
	{
		if (!m_Renderer->Draw(Material, DynamicOffsetNum)) return false;

		return true;
	}

	int CPrimitive::GetMaterialIndex()const
	{
		return m_MaterialIndex;
	}
}