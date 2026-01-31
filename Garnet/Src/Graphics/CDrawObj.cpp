#include "CDrawObj.h"

namespace graphics
{
	CDrawObj::CDrawObj(int RenderQueue, float ToCameraDist):
		m_RenderQueue(RenderQueue),
		m_ToCameraDist(ToCameraDist)
	{
	}

	CDrawObj::~CDrawObj()
	{
	}

	int CDrawObj::GetRenderQueue() const
	{
		return m_RenderQueue;
	}

	float CDrawObj::GetToCameraDist() const
	{
		return m_ToCameraDist;
	}

	bool CDrawObj::Draw()
	{
		return true;
	}
}