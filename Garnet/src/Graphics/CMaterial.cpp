#include "CMaterial.h"
#include "CUniformBufferDescriptor.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"

namespace graphics
{
	CMaterial::CMaterial():
		m_Renderer(nullptr)
	{

	}

	CMaterial::~CMaterial()
	{

	}

	bool CMaterial::Create(api::IGraphicsAPI* pGraphicsAPI, const renderer::CRendererCreateInfo& createInfo)
	{
		m_Renderer = pGraphicsAPI->CreateRenderer();
		if (!m_Renderer->Create(pGraphicsAPI, createInfo)) return false;

		return true;
	}
}