#include "CMesh.h"

namespace graphics
{
	CMesh::CMesh()
	{
	}

	CMesh::~CMesh()
	{
		Release();
	}

	bool CMesh::Release()
	{
		return true;
	}

	bool CMesh::Update(float SecondsTime)
	{
		for (const auto& Primitive : m_PrimitiveList)
		{
			if (!Primitive->Update(SecondsTime)) return false;
		}

		return true;
	}

	bool CMesh::Draw()
	{
		for (const auto& Primitive : m_PrimitiveList)
		{
			//const auto& Material = 
			//if (!Primitive->Draw()) return false;
		}

		return true;
	}

	void CMesh::AddPrimitive(const std::shared_ptr<CPrimitive>& Primitive)
	{
		m_PrimitiveList.push_back(Primitive);
	}
}