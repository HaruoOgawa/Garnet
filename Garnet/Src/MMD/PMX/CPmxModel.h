#pragma once

#ifdef USE_MMD
#include <vector>
#include <string>
#include <memory>

#include "EPmxEncodeType.h"
#include "EPmxWeightDeformFormat.h"
#include "SPmxMetaData.h"

#include "CPmxMesh.h"
#include "CPmxTexture.h"

namespace binary { class CBinaryAnalyser; }

namespace mmd
{
	class CPmxModel
	{
		std::shared_ptr<CPmxMesh> m_PmxMesh;
		std::vector<std::shared_ptr<CPmxTexture>> m_PmxTextureList;
	private:
		bool AnalyseMetaData(binary::CBinaryAnalyser& Analyser, SPmxMetaData& MetaData);

		bool AnalyseMesh(binary::CBinaryAnalyser& Analyser, const SPmxMetaData& MetaData);

		bool AnalyseTexture(binary::CBinaryAnalyser& Analyser, const SPmxMetaData& MetaData);

		// Helper Functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static bool GetMultiTypeValue(binary::CBinaryAnalyser& Analyser, int ByteSize, std::vector<int>& IntValueList, std::vector<unsigned char>& ByteValueList, std::vector<unsigned short>& UShortValueList);
		static bool AddEmptyMultiTypeValue(int ByteSize, std::vector<int>& IntValueList, std::vector<unsigned char>& ByteValueList, std::vector<unsigned short>& UShortValueList);
	public:
		CPmxModel();
		virtual ~CPmxModel();

		bool Analyse(const std::vector<unsigned char>& Data);
	};
}
#endif