#include "CMesh.h"
#include "CPrimitive.h"

namespace graphics
{
	CMesh::CMesh()
	{

	}

	CMesh::~CMesh()
	{

	}

	bool CMesh::Release()
	{
		return true;
	}

	bool CMesh::Update()
	{
		return true;
	}

	bool CMesh::Draw()
	{
		for (const auto& Primitive : m_PrimitiveList)
		{
			if (!Primitive->Draw()) return false;
		}

		return true;
	}

	void CMesh::AddPrimitive(const std::shared_ptr<CPrimitive>& Primitive)
	{
		m_PrimitiveList.push_back(Primitive);
	}
}