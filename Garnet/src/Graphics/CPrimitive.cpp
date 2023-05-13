#include "CPrimitive.h"
#include "CMaterial.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"

namespace graphics
{
	CPrimitive::CPrimitive(int MaterialIndex, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo, 
		EPresetPrimitiveType PresetType) :
		m_Renderer(nullptr),
		m_MaterialIndex(MaterialIndex),
		m_CreateInfo(createInfo),
		m_PresetType(PresetType),
		m_DynamicOffsetNum(0)
	{
		const auto& Material = MaterialList[MaterialIndex];
		Material->IncreaseRefCount();

		m_DynamicOffsetNum = Material->GetRefCount();
	}
	
	CPrimitive::~CPrimitive()
	{
	}

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList)
	{
		const auto& Material = MaterialList[m_MaterialIndex];

		if (m_PresetType == EPresetPrimitiveType::None)
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
		if (!m_Renderer->Create(pGraphicsAPI, createInfo, Material, m_DynamicOffsetNum)) return false;

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
		if (!m_Renderer->Create(pGraphicsAPI, createInfo, Material, m_DynamicOffsetNum)) return false;

		return true;
	}

	bool CPrimitive::Draw(const std::shared_ptr<CMaterial>& Material)
	{
		if (!m_Renderer->Draw(Material)) return false;

		return true;
	}
}