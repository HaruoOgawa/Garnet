#include "CPrimitive.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"
#include "CMaterial.h"
#include "../GraphicsAPI/CRendererCreateInfo.h"

namespace graphics
{
	CPrimitive::CPrimitive():
		m_Renderer(nullptr),
		m_Material(nullptr)
	{

	}

	CPrimitive::~CPrimitive()
	{

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

	bool CPrimitive::Update()
	{
		return true;
	}

	bool CPrimitive::Draw()
	{
		if (!m_Renderer->Draw()) return false;

		return true;
	}
}