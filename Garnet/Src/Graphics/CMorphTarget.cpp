#include "CMorphTarget.h"

namespace graphics
{
	CMorphTarget::CMorphTarget()
	{
	}

	void CMorphTarget::AddMorphData(const std::string& AttributeName, const SMorphData& Data)
	{
		if (m_MorphDataMap.find(AttributeName) == m_MorphDataMap.end())
		{
			m_MorphDataMap.emplace(AttributeName, std::vector<SMorphData>());
		}

		m_MorphDataMap[AttributeName].push_back(Data);
	}

	const std::map<std::string, std::vector<SMorphData>>& CMorphTarget::GetMorphDataMap() const
	{
		return m_MorphDataMap;
	}
}