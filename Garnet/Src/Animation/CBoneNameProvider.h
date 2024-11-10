#pragma once
#ifdef USE_ANIMATION

#include <unordered_map>
#include <map>
#include <vector>
#include <string>

#include "EHumanoidBones.h"
#include "EBonePattern.h"

namespace animation
{
	class CBoneNameProvider
	{
		std::unordered_map<EHumanoidBones, std::vector<std::string>> m_BoneNameTable;
		std::map<EBonePattern, std::map<std::wstring, EHumanoidBones>> m_TableListU16;
		
	private:
		void InitTable();
		void InitTableU16();

		void InitPMXTable();
		void InitVMDTable();

	public:
		CBoneNameProvider();
		virtual ~CBoneNameProvider() = default;

		EHumanoidBones GetBoneName(const std::string& SrcNodeName);
		EHumanoidBones GetBoneNameU16(EBonePattern Pattern, const std::wstring& SrcNodeName);

		static std::wstring HexToWstr(std::vector<int> byteArray);
		static std::wstring ToNoZeroStr(const std::wstring& Src);
	};
}

#endif // USE_ANIMATION