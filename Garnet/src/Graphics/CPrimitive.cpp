#include "CPrimitive.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IVertex.h"
#include "../GraphicsAPI/CVertexCreateInfo.h"

namespace graphics
{
	CPrimitive::CPrimitive(api::IGraphicsAPI* pGraphicsAPI, const vertex::CVertexCreateInfo& createInfo, int MaterialIndex):
		m_Vertex(nullptr),
		m_MaterialIndex(MaterialIndex)
	{
		Create(pGraphicsAPI, createInfo);
	}
	
	CPrimitive::CPrimitive(api::IGraphicsAPI* pGraphicsAPI, EPresetPrimitiveType Type, int MaterialIndex):
		m_Vertex(nullptr),
		m_MaterialIndex(MaterialIndex)
	{
		Create(pGraphicsAPI, Type);
	}

	CPrimitive::~CPrimitive()
	{
	}

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, const vertex::CVertexCreateInfo& createInfo)
	{
		m_Vertex = pGraphicsAPI->CreateVertex();
		if (!m_Vertex->Create(pGraphicsAPI, createInfo)) return false;

		return true;
	}

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, EPresetPrimitiveType Type)
	{
		vertex::CVertexCreateInfo createInfo;

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

		m_Vertex = pGraphicsAPI->CreateVertex();
		if (!m_Vertex->Create(pGraphicsAPI, createInfo)) return false;

		return true;
	}
}