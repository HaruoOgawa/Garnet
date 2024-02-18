#include "CMesh.h"

namespace graphics
{
	CMesh::CMesh()
	{
	}

	CMesh::~CMesh()
	{
	}

	void CMesh::AddPrimitive(const std::shared_ptr<CPrimitive>& Primitive)
	{
		m_PrimitiveList.push_back(Primitive);
	}

	const std::vector<std::shared_ptr<CPrimitive>>& CMesh::GetPrimitiveList() const
	{
		return m_PrimitiveList;
	}

	void CMesh::CreateSimpleMesh(const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo, int MaterialIndex)
	{
		std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(createInfo, MaterialIndex);
		AddPrimitive(Primitive);
	}
}