#include "CPrimitive.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include "CMaterial.h"
#include "CTextureSet.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"
#include "../GraphicsAPI/SDrawObj.h"

namespace graphics
{
	CPrimitive::CPrimitive(const std::shared_ptr<CVertexBuffer>& VertexBuffer, const std::shared_ptr<CIndexBuffer>& IndexBuffer) :
		m_Enabled(true),
		m_PresetType(graphics::EPresetPrimitiveType::None),
		m_VertexBuffer(VertexBuffer),
		m_IndexBuffer(IndexBuffer),
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

	bool CPrimitive::Create(const std::vector<std::string>& PassNameList, const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		if (!m_VertexBuffer || !m_IndexBuffer) return false;

		if (m_RendererList.empty()) return false;

		for (auto& RendererMat : m_RendererList)
		{
			if (!std::get<1>(RendererMat)->Create(PassNameList, TextureSet)) return false;

			if (!std::get<0>(RendererMat)->Create(PassNameList, m_VertexBuffer, m_IndexBuffer, std::get<1>(RendererMat))) return false;
		}

		return true;
	}

	bool CPrimitive::Draw(api::IGraphicsAPI* pGraphicsAPI, const glm::mat4& WorldMatrix, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection)
	{
		if (!IsEnabled()) return true;

		for (const auto& RendererMat : m_RendererList)
		{
#ifdef USE_DRAW_SORT
			const int RenderQueue = std::get<1>(RendererMat)->GetRenderQueue();

			glm::vec3 WorldPos = glm::vec3(WorldMatrix[3][0], WorldMatrix[3][1], WorldMatrix[3][2]);
			float ToCameraDist = glm::distance(Camera->GetPos(), WorldPos);

			api::SDrawObj DrawObj = { RenderQueue, ToCameraDist, RendererMat, m_VertexBuffer, m_IndexBuffer, WorldMatrix };
			if (!pGraphicsAPI->AddDrawObj(DrawObj)) return false;
#else
			if (!std::get<0>(RendererMat)->Draw(m_VertexBuffer, m_IndexBuffer, std::get<1>(RendererMat))) return false;
#endif // USE_DRAW_SORT
		}
		
		return true;
	}

	const std::vector<std::tuple<std::shared_ptr<graphics::IRenderer>, std::shared_ptr<CMaterial>>>& CPrimitive::GetRendererList() const
	{
		return m_RendererList;
	}

	void CPrimitive::ReplaceMaterial(const std::tuple<std::shared_ptr<graphics::IRenderer>, std::shared_ptr<CMaterial>>& Renderer, const std::shared_ptr<CMaterial>& NewMaterial)
	{
		for (int i = 0; i < static_cast<int>(m_RendererList.size()); i++)
		{
			if (m_RendererList[i] != Renderer) continue;

			m_RendererList[i] = std::make_tuple(std::get<0>(m_RendererList[i]), NewMaterial);
			break;
		}
	}

	void CPrimitive::AddMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CMaterial>& Material)
	{
		if (!Material) return;

		m_RendererList.push_back(std::make_tuple(pGraphicsAPI->CreateRenderer(), Material));
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