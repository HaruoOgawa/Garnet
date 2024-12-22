#pragma once

#include <memory>
#include <vector>
#include <string>

namespace graphics
{
	class CMaterial;
	class CVertexBuffer;
	class CIndexBuffer;

	class IRenderer
	{
	public:
		virtual bool Create(const std::vector<std::string>& PassNameList, const std::shared_ptr<CVertexBuffer>& VertexBuffer,
			const std::shared_ptr<CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material) = 0;
		virtual bool Draw(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, 
			const std::shared_ptr<graphics::CMaterial>& Material) = 0;
	
		virtual bool UpdateVertexBuffer(const std::vector<float>& PosAttribute, const std::shared_ptr<CVertexBuffer>& VertexBuffer) = 0;
	};
}