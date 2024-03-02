#pragma once

#include <vector>
#include <map>
#include <string>

namespace graphics
{
	struct SMorphData
	{
		int m_VertexIndex;
		std::vector<float> m_OffsetVector;
		float m_Weight;
	};

	class CMorphTarget
	{
		std::map<std::string, std::vector<SMorphData>> m_MorphDataList;
	public:
		CMorphTarget();
		virtual ~CMorphTarget() = default;

		void AddMorphData(const std::string& AttributeName, const SMorphData& Data);
	};
}