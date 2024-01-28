#pragma once
#ifdef USE_ANIMATION

#include <unordered_map>
#include <vector>
#include <string>

#include "EHumanoidBones.h"

namespace animation
{
	class CBoneNameProvider
	{
		std::unordered_map<EHumanoidBones, std::vector<std::string>> m_BoneNameTable;
		std::unordered_map<EHumanoidBones, std::vector<std::wstring>> m_BoneNameTableU16;
		
	private:
		void InitTable();
		void InitTableU16();
	public:
		CBoneNameProvider();
		virtual ~CBoneNameProvider() = default;

		EHumanoidBones GetBoneName(const std::string& SrcNodeName);
		EHumanoidBones GetBoneNameU16(const std::wstring& SrcNodeName);
	};
}

#endif // USE_ANIMATION