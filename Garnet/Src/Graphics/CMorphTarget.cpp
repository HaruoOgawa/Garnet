#include "CMorphTarget.h"

namespace graphics
{
	CMorphTarget::CMorphTarget()
	{
	}

	void CMorphTarget::AddMorphData(const std::string& AttributeName, const SMorphData& Data)
	{
		if (m_MorphDataList.find(AttributeName) == m_MorphDataList.end())
		{
			m_MorphDataList.emplace(AttributeName, std::vector<SMorphData>());
		}

		m_MorphDataList[AttributeName].push_back(Data);
	}
}