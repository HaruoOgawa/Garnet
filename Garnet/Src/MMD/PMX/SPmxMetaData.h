#pragma once

#ifdef USE_MMD

#include <vector>
#include <string>
#include "EPmxEncodeType.h"
#include "EPmxWeightDeformFormat.h"

namespace mmd
{
	struct SPmxMetaData
	{
		EPmxEncodeType EncodeType = EPmxEncodeType::UTF16;

		int AdditionalUVCount = 0;
		int VertexIndexSize = 0;
		int TextureIndexSize = 0;
		int MaterialIndexSize = 0;
		int BoneIndexSize = 0;
		int MorphIndexSize = 0;
		int RigidIndexSize = 0;

		std::pair<std::string, std::wstring> ModelName = std::make_pair(std::string(), std::wstring());
		std::pair<std::string, std::wstring> ModelName_EN = std::make_pair(std::string(), std::wstring());
		std::pair<std::string, std::wstring> Comment = std::make_pair(std::string(), std::wstring());
		std::pair<std::string, std::wstring> Comment_EN = std::make_pair(std::string(), std::wstring());
	};
}
#endif