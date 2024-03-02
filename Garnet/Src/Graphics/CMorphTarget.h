#pragma once

#include <vector>
#include <map>
#include <string>

namespace graphics
{
	struct SMorphData
	{
		int VertexIndex;
		std::vector<float> OffsetVector;
		float InitialWeight;
	};

	class CMorphTarget
	{
		std::map<std::string, std::vector<SMorphData>> m_MorphDataMap;
	public:
		CMorphTarget();
		virtual ~CMorphTarget() = default;

		void AddMorphData(const std::string& AttributeName, const SMorphData& Data);

		const std::map<std::string, std::vector<SMorphData>>& GetMorphDataMap() const;
	};
}