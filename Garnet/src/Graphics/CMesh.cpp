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
}