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
#include "CPmxMaterial.h"

namespace binary { class CBinaryAnalyser; }

namespace mmd
{
	class CPmxModel
	{
		SPmxMetaData m_MetaData;

		std::shared_ptr<CPmxMesh> m_PmxMesh;
		std::vector<std::shared_ptr<CPmxTexture>> m_PmxTextureList;
		std::vector<std::shared_ptr<CPmxMaterial>> m_PmxMaterialList;
	private:
		bool AnalyseMetaData(binary::CBinaryAnalyser& Analyser, SPmxMetaData& MetaData);

		bool AnalyseMesh(binary::CBinaryAnalyser& Analyser, const SPmxMetaData& MetaData);

		bool AnalyseTexture(binary::CBinaryAnalyser& Analyser, const SPmxMetaData& MetaData);

		bool AnalyseMaterial(binary::CBinaryAnalyser& Analyser, const SPmxMetaData& MetaData);

		// Helper Functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static bool GetMultiTypeValue(binary::CBinaryAnalyser& Analyser, int ByteSize, std::vector<unsigned int>& UIntValueList, std::vector<unsigned char>& ByteValueList, std::vector<unsigned short>& UShortValueList);
		static bool AddEmptyMultiTypeValue(int ByteSize, std::vector<unsigned int>& UIntValueList, std::vector<unsigned char>& ByteValueList, std::vector<unsigned short>& UShortValueList);

		static int GetMultiTypeValueAsInterger(binary::CBinaryAnalyser& Analyser, int ByteSize);
	public:
		CPmxModel();
		virtual ~CPmxModel();

		const SPmxMetaData& GetMetaData() const;

		const std::shared_ptr<CPmxMesh>& GetPmxMesh() const;

		const std::vector<std::shared_ptr<CPmxTexture>>& GetPmxTextureList() const;

		const std::vector<std::shared_ptr<CPmxMaterial>>& GetPmxMaterialList() const;

		bool Analyse(const std::vector<unsigned char>& Data);
	};
}
#endif