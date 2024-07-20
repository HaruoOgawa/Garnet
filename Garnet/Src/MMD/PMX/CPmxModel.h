#pragma once

#if defined(USE_MMD)  && defined(USE_BINARY_READ)
#include <vector>
#include <map>
#include <string>
#include <memory>

#include "EPmxEncodeType.h"
#include "EPmxWeightDeformFormat.h"
#include "SPmxMetaData.h"

#include "CPmxMesh.h"
#include "CPmxTexture.h"
#include "CPmxMaterial.h"
#include "CPmxBone.h"
#include "CPmxMorphTarget.h"
#include "SPmxRigidbody.h"
#include "SPmxJoint.h"

#include "../../Animation/EBlendShapeName.h"

namespace binary { class CBinaryReader; }

namespace mmd
{
	class CPmxModel
	{
		SPmxMetaData m_MetaData;

		std::shared_ptr<CPmxMesh> m_PmxMesh;
		std::vector<std::shared_ptr<CPmxTexture>> m_PmxTextureList;
		std::vector<std::shared_ptr<CPmxMaterial>> m_PmxMaterialList;
		std::vector<std::shared_ptr<CPmxBone>> m_PmxBoneList;

		// 頂点モーフリスト
		std::map<animation::EBlendShapeName, std::shared_ptr<CPmxMorphTarget>> m_PmxVertexMorphList; 

		// ボーンモーフリスト
		// 未実装

		// UVモーフリスト
		// 未実装

		// 追加UV1モーフリスト
		// 未実装

		// 追加UV2モーフリスト
		// 未実装

		// 追加UV3モーフリスト
		// 未実装

		// 追加UV4モーフリスト
		// 未実装

		// 材質モーフリスト
		// 未実装
		
		std::vector<SPmxRigidbody> m_PmxRigidbodyList;
		std::vector<SPmxJoint> m_PmxJointList;
	private:
		bool AnalyseMetaData(binary::CBinaryReader& Analyser, SPmxMetaData& MetaData);

		bool AnalyseMesh(binary::CBinaryReader& Analyser, const SPmxMetaData& MetaData);

		bool AnalyseTexture(binary::CBinaryReader& Analyser, const SPmxMetaData& MetaData);

		bool AnalyseMaterial(binary::CBinaryReader& Analyser, const SPmxMetaData& MetaData);
		
		bool AnalyseBone(binary::CBinaryReader& Analyser, const SPmxMetaData& MetaData);

		bool AnalyseMorph(binary::CBinaryReader& Analyser, const SPmxMetaData& MetaData);

		bool AnalyseDisplayFrame(binary::CBinaryReader& Analyser, const SPmxMetaData& MetaData);

		bool AnalyseRigidbody(binary::CBinaryReader& Analyser, const SPmxMetaData& MetaData);

		bool AnalyseJoint(binary::CBinaryReader& Analyser, const SPmxMetaData& MetaData);

		// Helper Functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static bool GetMultiTypeValue(binary::CBinaryReader& Analyser, int ByteSize, std::vector<unsigned int>& UIntValueList, std::vector<unsigned char>& ByteValueList, std::vector<unsigned short>& UShortValueList);
		static bool AddEmptyMultiTypeValue(int ByteSize, std::vector<unsigned int>& UIntValueList, std::vector<unsigned char>& ByteValueList, std::vector<unsigned short>& UShortValueList);

		static int GetMultiTypeValueAsInterger(binary::CBinaryReader& Analyser, int ByteSize);
	public:
		CPmxModel();
		virtual ~CPmxModel();

		const SPmxMetaData& GetMetaData() const;

		const std::shared_ptr<CPmxMesh>& GetPmxMesh() const;

		const std::vector<std::shared_ptr<CPmxTexture>>& GetPmxTextureList() const;

		const std::vector<std::shared_ptr<CPmxMaterial>>& GetPmxMaterialList() const;

		const std::vector<std::shared_ptr<CPmxBone>>& GetPmxBoneList() const;

		const std::map<animation::EBlendShapeName, std::shared_ptr<CPmxMorphTarget>>& GetPmxVertexMorphList() const;

		const std::vector<SPmxRigidbody>& GetPmxRigidbodyList() const;

		const std::vector<SPmxJoint>& GetPmxJointList() const;

		bool Analyse(const std::vector<unsigned char>& Data);
	};
}
#endif