#include "CPrimitive.h"
#include "CMaterial.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"
#include "../GraphicsAPI/CRendererCreateInfo.h"

namespace graphics
{
	CPrimitive::CPrimitive(api::IGraphicsAPI* pGraphicsAPI, const renderer::CRendererCreateInfo& createInfo, int MaterialIndex):
		m_Renderer(nullptr),
		m_MaterialIndex(MaterialIndex)
	{
		Create(pGraphicsAPI, createInfo);
	}
	
	CPrimitive::CPrimitive(api::IGraphicsAPI* pGraphicsAPI, EPresetPrimitiveType Type, int MaterialIndex):
		m_Renderer(nullptr),
		m_MaterialIndex(MaterialIndex)
	{
		Create(pGraphicsAPI, Type);
	}

	CPrimitive::~CPrimitive()
	{
		Release();
	}

	bool CPrimitive::Release()
	{
		return true;
	}

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, const renderer::CRendererCreateInfo& createInfo)
	{
		m_Renderer = pGraphicsAPI->CreateRenderer();
		if (!m_Renderer->Create(pGraphicsAPI, createInfo)) return false;

		return true;
	}

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, EPresetPrimitiveType Type)
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
		if (!m_Renderer->Create(pGraphicsAPI, createInfo)) return false;

		return true;
	}

	bool CPrimitive::Update(float SecondsTime)
	{
		if (!m_Renderer->Update(SecondsTime)) return false;

		return true;
	}

	bool CPrimitive::Draw()
	{
		if (!m_Renderer->Draw()) return false;

		return true;
	}
}