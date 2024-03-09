#pragma once

#ifdef USE_ANIMATION

#include <string>
#include <unordered_map>
#include "EBlendShapeName.h"

namespace animation
{
	class CBlendShapeNameProvider
	{
		std::unordered_map<EBlendShapeName, std::vector<std::string>> m_BlendShapeNameTable;
		std::unordered_map<EBlendShapeName, std::vector<std::wstring>> m_BlendShapeNameTableU16;

	private:
		void InitTable();
		void InitTableU16();

	public:
		CBlendShapeNameProvider();
		virtual ~CBlendShapeNameProvider() = default;

		EBlendShapeName GetBlendShapeName(const std::string& SrcNodeName);
		EBlendShapeName GetBlendShapeNameU16(const std::wstring& SrcNodeName);

		static std::wstring HexToWstr(std::vector<int> byteArray);
	};
}
#endif