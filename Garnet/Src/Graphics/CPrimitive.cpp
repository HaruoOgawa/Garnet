#include "CPrimitive.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include "CMaterial.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"

namespace graphics
{
	CPrimitive::CPrimitive(const std::shared_ptr<CVertexBuffer>& VertexBuffer, const std::shared_ptr<CIndexBuffer>& IndexBuffer, int MaterialIndex) :
		m_PresetType(graphics::EPresetPrimitiveType::None),
		m_VertexBuffer(VertexBuffer),
		m_IndexBuffer(IndexBuffer),
		m_Renderer(nullptr),
		m_DepthRenderer(nullptr),
		m_MaterialIndex(MaterialIndex),
		m_UseMorph(false)
	{
	}
	
	CPrimitive::~CPrimitive()
	{
	}

	void CPrimitive::SetPresetType(graphics::EPresetPrimitiveType Type)
	{
		m_PresetType = Type;
	}

	graphics::EPresetPrimitiveType CPrimitive::GetPresetType() const
	{
		return m_PresetType;
	}

	void CPrimitive::Release()
	{
		// 生成処理が終わったので不要なリソースを解放する
		// (モーフが使っていなければ)
		if (!m_UseMorph)
		{
			m_VertexBuffer->Release();
			m_IndexBuffer->Release();
		}
	}

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, const std::string& PassName, const std::shared_ptr<graphics::CMaterial>& Material, bool IsDepth)
	{
		if (!m_VertexBuffer || !m_IndexBuffer) return false;

		if (IsDepth)
		{
			m_DepthRenderer = pGraphicsAPI->CreateRenderer(PassName);
			if (!m_DepthRenderer->Create(m_VertexBuffer, m_IndexBuffer, Material)) return false;
		}
		else
		{
			m_Renderer = pGraphicsAPI->CreateRenderer(PassName);
			if (!m_Renderer->Create(m_VertexBuffer, m_IndexBuffer, Material)) return false;
		}

		return true;
	}

	bool CPrimitive::Draw(const std::shared_ptr<CMaterial>& Material, int DynamicOffsetNum, bool IsDepth)
	{
		if (IsDepth)
		{
			if (!m_DepthRenderer->Draw(m_VertexBuffer, m_IndexBuffer, Material, DynamicOffsetNum)) return false;
		}
		else
		{
			if (!m_Renderer->Draw(m_VertexBuffer, m_IndexBuffer, Material, DynamicOffsetNum)) return false;
		}
		
		return true;
	}

	const std::shared_ptr<graphics::IRenderer>& CPrimitive::GetRenderer() const
	{
		return m_Renderer;
	}

	void CPrimitive::SetMaterialIndex(int Index)
	{
		m_MaterialIndex = Index;
	}

	int CPrimitive::GetMaterialIndex()const
	{
		return m_MaterialIndex;
	}

	const std::shared_ptr<CVertexBuffer>& CPrimitive::GetVertexBuffer() const
	{
		return m_VertexBuffer;
	}

	// モーフ
	void CPrimitive::SetUseMorph(bool Flag)
	{
		m_UseMorph = Flag;
	}
}