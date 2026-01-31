#pragma once

#include <memory>
#include <tuple>
#include <glm/glm.hpp>
#include "CDrawObj.h"

namespace graphics
{
	class IRenderer;
	class CMaterial;
	class CVertexBuffer;
	class CIndexBuffer;
}

namespace graphics
{
	class C3DDrawObj : public CDrawObj
	{
		std::shared_ptr<graphics::IRenderer> m_Renderer;
		std::shared_ptr<graphics::CMaterial> m_Materail;

		std::shared_ptr<graphics::CVertexBuffer> m_VertexBuffer;
		std::shared_ptr<graphics::CIndexBuffer> m_IndexBuffer;

		glm::mat4 m_WorldMatrix;

	public:
		C3DDrawObj(
			int RenderQueue, float ToCameraDist,
			const std::shared_ptr<graphics::IRenderer>& Renderer,
			const std::shared_ptr<graphics::CMaterial>& Materail,
			const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer,
			const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, 
			const glm::mat4& WorldMatrix);

		virtual ~C3DDrawObj() = default;

		virtual bool Draw() override;
	};
}