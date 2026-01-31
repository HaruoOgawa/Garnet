#include "C3DDrawObj.h"
#include "../../Interface/IRenderer.h"
#include "../../Graphics/CMaterial.h"
#include "../../Graphics/CVertexBuffer.h"
#include "../../Graphics/CIndexBuffer.h"

namespace graphics
{
	C3DDrawObj::C3DDrawObj(
		int RenderQueue, float ToCameraDist,
		const std::shared_ptr<graphics::IRenderer>& Renderer,
		const std::shared_ptr<graphics::CMaterial>& Materail,
		const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer,
		const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer,
		const glm::mat4& WorldMatrix) :
		CDrawObj(RenderQueue, ToCameraDist),
		m_Renderer(Renderer),
		m_Materail(Materail),
		m_VertexBuffer(VertexBuffer),
		m_IndexBuffer(IndexBuffer),
		m_WorldMatrix(WorldMatrix)
	{
	}

	bool C3DDrawObj::Draw()
	{
		m_Materail->SetUniformValue("model", &m_WorldMatrix[0][0], sizeof(glm::mat4));

		if (!m_Renderer->Draw(m_VertexBuffer, m_IndexBuffer, m_Materail)) return false;

		return true;
	}
}