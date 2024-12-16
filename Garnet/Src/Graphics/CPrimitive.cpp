#include "CPrimitive.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include "CMaterial.h"
#include "CTextureSet.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"

namespace graphics
{
	CPrimitive::CPrimitive(const std::shared_ptr<CVertexBuffer>& VertexBuffer, const std::shared_ptr<CIndexBuffer>& IndexBuffer, const std::shared_ptr<CMaterial>& Material) :
		m_Enabled(true),
		m_PresetType(graphics::EPresetPrimitiveType::None),
		m_VertexBuffer(VertexBuffer),
		m_IndexBuffer(IndexBuffer),
		m_Renderer(nullptr),
		m_Material(Material),
		m_UseMorph(false)
	{
	}
	
	CPrimitive::~CPrimitive()
	{
	}

	void CPrimitive::SetEnabled(bool Flag)
	{
		m_Enabled = Flag;
	}

	bool CPrimitive::IsEnabled() const
	{
		return m_Enabled;
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

	bool CPrimitive::Create(api::IGraphicsAPI* pGraphicsAPI, const std::string& PassName, const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		if (!m_VertexBuffer || !m_IndexBuffer) return false;

		if (!m_Material) return false;

		if (!m_Material->Create(TextureSet)) return false;

		m_Renderer = pGraphicsAPI->CreateRenderer(PassName);
		if (!m_Renderer->Create(m_VertexBuffer, m_IndexBuffer, m_Material)) return false;

		return true;
	}

	bool CPrimitive::Draw(int DynamicOffsetNum)
	{
		if (!IsEnabled()) return true;

		if (!m_Renderer->Draw(m_VertexBuffer, m_IndexBuffer, m_Material, DynamicOffsetNum)) return false;
		
		return true;
	}

	const std::shared_ptr<graphics::IRenderer>& CPrimitive::GetRenderer() const
	{
		return m_Renderer;
	}

	void CPrimitive::AddMaterial(const std::shared_ptr<CMaterial>& Material)
	{
		m_Material = Material;
	}

	const std::shared_ptr<CMaterial>& CPrimitive::GetMaterial() const
	{
		return m_Material;
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