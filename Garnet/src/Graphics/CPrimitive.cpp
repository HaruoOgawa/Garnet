#include "CPrimitive.h"
#include "CMaterial.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"
#include "../GraphicsAPI/CRendererCreateInfo.h"

namespace graphics
{
	CPrimitive::CPrimitive(api::IGraphicsAPI* pGraphicsAPI, const renderer::CRendererCreateInfo& createInfo, int MaterialIndex, const std::vector<std::shared_ptr<CMaterial>>& MaterialList):
		m_Renderer(nullptr),
		m_MaterialIndex(MaterialIndex)
	{
		Create(pGraphicsAPI, createInfo, MaterialList[MaterialIndex]);
	}
	
	CPrimitive::CPrimitive(api::IGraphicsAPI* pGraphicsAPI, EPresetPrimitiveType Type, int MaterialIndex, const std::vector<std::shared_ptr<CMaterial>>& MaterialList):
		m_Renderer(nullptr),
		m_MaterialIndex(MaterialIndex)
	{
		Create(pGraphicsAPI, Type, MaterialList[MaterialIndex]);
	}

	CPrimitive::~CPrimitive()
	{
	}

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, const renderer::CRendererCreateInfo& createInfo, const std::shared_ptr<CMaterial>& Material)
	{
		m_Renderer = pGraphicsAPI->CreateRenderer();
		if (!m_Renderer->Create(pGraphicsAPI, createInfo, Material)) return false;

		return true;
	}

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, EPresetPrimitiveType Type, const std::shared_ptr<CMaterial>& Material)
	{
		renderer::CRendererCreateInfo createInfo;

		switch (Type)
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
		if (!m_Renderer->Create(pGraphicsAPI, createInfo, Material)) return false;

		return true;
	}

	bool CPrimitive::Update(float SecondsTime)
	{
		return true;
	}

	bool CPrimitive::Draw()
	{
		return true;
	}
}