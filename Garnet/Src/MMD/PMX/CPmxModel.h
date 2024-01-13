#pragma once

#ifdef USE_MMD
#include <vector>
#include <string>

namespace binary { class CBinaryAnalyser; }

namespace mmd
{
	enum class EEncodeType
	{
		UTF16 = 0,
		UTF8 = 1,
	};

	// Joints, Weghts‚ÌŠi”[•û–@
	enum class EWeightDeformFormat
	{
		None = -1,

		BDEF1 = 0,
		BDEF2 = 1,
		BDEF4 = 2,
		SDEF = 3,
	};

	struct SMetaData
	{
		EEncodeType EncodeType = EEncodeType::UTF16;
		
		int AdditionalUVCount = 0;
		int VertexIndexSize = 0;
		int TextureIndexSize = 0;
		int MaterialIndexSize = 0;
		int BoneIndexSize = 0;
		int MorphIndexSize = 0;
		int RigidIndexSize = 0;

		std::string ModelName = "";
		std::string ModelName_EN = "";
		std::string Comment = "";
		std::string Comment_EN = "";
	};

	class CPmxModel
	{
	private:
		static bool AnalyseMetaData(binary::CBinaryAnalyser& Analyser, SMetaData& MetaData);

		static bool AnalyseMesh(binary::CBinaryAnalyser& Analyser, const SMetaData& MetaData);

		static bool AnalyseTexture(binary::CBinaryAnalyser& Analyser, const SMetaData& MetaData);

		// Helper Functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static bool GetMultiTypeValue(binary::CBinaryAnalyser& Analyser, int ByteSize, std::vector<int>& IntValueList, std::vector<unsigned char>& ByteValueList, std::vector<unsigned short>& UShortValueList);
		static bool AddEmptyMultiTypeValue(int ByteSize, std::vector<int>& IntValueList, std::vector<unsigned char>& ByteValueList, std::vector<unsigned short>& UShortValueList);
	public:
		static bool Analyse(const std::vector<unsigned char>& Data);
	};
}
#endif