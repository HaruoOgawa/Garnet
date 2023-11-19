#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "EHumanoidBones.h"

namespace animation
{
	class CBoneNameProvider
	{
		std::unordered_map<EHumanoidBones, std::vector<std::string>> m_BoneNameTable;
	private:
		void InitTable();
	public:
		CBoneNameProvider();
		virtual ~CBoneNameProvider() = default;

		EHumanoidBones GetBoneName(const std::string& SrcNodeName);
	};
}