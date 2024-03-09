#pragma once
#include <vector>
#include <memory>
#include "CPrimitive.h"

namespace graphics
{
	class CPrimitive;
	class CVertexBuffer;
	class CIndexBuffer;

	class CMesh
	{
		std::vector<std::shared_ptr<CPrimitive>> m_PrimitiveList;

		std::vector<std::shared_ptr<CVertexBuffer>> m_VertexBufferList;
		std::vector<std::shared_ptr<CIndexBuffer>> m_IndexBufferList;
	public:
		CMesh();
		virtual ~CMesh();

		bool CreateBuffer();

		void AddVertexBuffer(const std::shared_ptr<CVertexBuffer>& Buffer);
		void AddIndexBuffer(const std::shared_ptr<CIndexBuffer>& Buffer);

		void AddPrimitive(const std::shared_ptr<CPrimitive>& Primitive);
		const std::vector<std::shared_ptr<CPrimitive>>& GetPrimitiveList() const;

		void CreateSimpleMesh(const std::shared_ptr<CVertexBuffer>& VertexBuffer, const std::shared_ptr<CIndexBuffer>& IndexBuffer, int MaterialIndex);
	};
}