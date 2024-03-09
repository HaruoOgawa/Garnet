#include "CMesh.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"

namespace graphics
{
	CMesh::CMesh()
	{
	}

	CMesh::~CMesh()
	{
	}

	bool CMesh::CreateBuffer()
	{
		for (auto& VertexBuffer : m_VertexBufferList)
		{
			if (!VertexBuffer->Create()) return false;
		}

		for (auto& IndexBuffer : m_IndexBufferList)
		{
			if (!IndexBuffer->Create()) return false;
		}

		return true;
	}

	void CMesh::AddVertexBuffer(const std::shared_ptr<CVertexBuffer>& Buffer)
	{
		m_VertexBufferList.push_back(Buffer);
	}

	void CMesh::AddIndexBuffer(const std::shared_ptr<CIndexBuffer>& Buffer)
	{
		m_IndexBufferList.push_back(Buffer);
	}

	void CMesh::AddPrimitive(const std::shared_ptr<CPrimitive>& Primitive)
	{
		m_PrimitiveList.push_back(Primitive);
	}

	const std::vector<std::shared_ptr<CPrimitive>>& CMesh::GetPrimitiveList() const
	{
		return m_PrimitiveList;
	}

	void CMesh::CreateSimpleMesh(const std::shared_ptr<CVertexBuffer>& VertexBuffer, const std::shared_ptr<CIndexBuffer>& IndexBuffer, int MaterialIndex)
	{
		AddVertexBuffer(VertexBuffer);
		AddIndexBuffer(IndexBuffer);

		std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(VertexBuffer, IndexBuffer, MaterialIndex);
		AddPrimitive(Primitive);
	}
}