#pragma once

#ifdef USE_ANIMATION

#include <string>
#include <unordered_map>
#include <map>
#include "EBlendShapeName.h"
#include "EBonePattern.h"

namespace animation
{
	class CBlendShapeNameProvider
	{
		std::unordered_map<EBlendShapeName, std::vector<std::string>> m_BlendShapeNameTable;
		std::map<EBonePattern, std::map<std::wstring, EBlendShapeName>> m_TableListU16;

	private:
		void InitTable();
		void InitTableU16();

		void InitPMXTable();
		void InitVMDTable();

	public:
		CBlendShapeNameProvider();
		virtual ~CBlendShapeNameProvider() = default;

		EBlendShapeName GetBlendShapeName(const std::string& SrcNodeName);
		EBlendShapeName GetBlendShapeNameU16(EBonePattern Pattern, const std::wstring& SrcNodeName);

		static std::wstring HexToWstr(std::vector<int> byteArray);
		static std::wstring ToNoZeroStr(const std::wstring& Src);
	};
}
#endif