#include "CPrimitive.h"
#include "CMaterial.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"

namespace graphics
{
	CPrimitive::CPrimitive(const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo, int MaterialIndex) :
		m_Renderer(nullptr),
		m_DepthRenderer(nullptr),
		m_CreateInfo(createInfo),
		m_MaterialIndex(MaterialIndex)
	{
	}
	
	CPrimitive::~CPrimitive()
	{
	}

	void CPrimitive::Release()
	{
		// 生成処理が終わったので不要なリソースを解放する
		m_CreateInfo = nullptr;
	}

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, const std::string& PassName, const std::shared_ptr<graphics::CMaterial>& Material, bool IsDepth)
	{
		if (!m_CreateInfo) return false;

		if (IsDepth)
		{
			m_DepthRenderer = pGraphicsAPI->CreateRenderer(PassName);
			if (!m_DepthRenderer->Create(m_CreateInfo, Material)) return false;
		}
		else
		{
			m_Renderer = pGraphicsAPI->CreateRenderer(PassName);
			if (!m_Renderer->Create(m_CreateInfo, Material)) return false;
		}

		return true;
	}

	bool CPrimitive::Draw(const std::shared_ptr<CMaterial>& Material, int DynamicOffsetNum, bool IsDepth)
	{
		if (IsDepth)
		{
			if (!m_DepthRenderer->Draw(Material, DynamicOffsetNum)) return false;
		}
		else
		{
			if (!m_Renderer->Draw(Material, DynamicOffsetNum)) return false;
		}
		
		return true;
	}

	int CPrimitive::GetMaterialIndex()const
	{
		return m_MaterialIndex;
	}
}