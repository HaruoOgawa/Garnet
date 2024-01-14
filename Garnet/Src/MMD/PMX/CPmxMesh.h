#pragma once

#ifdef USE_MMD
#include <vector>
#include <string>
namespace mmd
{
	class CPmxMesh
	{
		std::vector<float> m_PositionAttribute;
		std::vector<float> m_NormalAttribute;
		std::vector<float> m_UVAttribute;
		std::vector<float> m_TangentAttribute;

		// MetaData.BoneIndexSizeに応じてバイト数が変わる
		std::vector<int> m_IntJointAttribute;
		std::vector<unsigned char> m_ByteJointAttribute;
		std::vector<unsigned short> m_UShortJointAttribute;

		std::vector<float> m_WeightAttribute;

		std::vector<std::vector<float>> m_AdditionalUVAttribute;

		// インデックスバッファ
		// MetaData.VertexIndexSizeに応じてバイト数が変わる
		std::vector<int> m_IntIndices;
		std::vector<unsigned char> m_ByteIndices;
		std::vector<unsigned short> m_UShortIndices;

	public:
		CPmxMesh(const std::vector<float>& PositionAttribute, const std::vector<float>& NormalAttribute, const std::vector<float>& UVAttribute, const std::vector<float>& TangentAttribute, const std::vector<int>& IntJointAttribute, 
			const std::vector<unsigned char>& ByteJointAttribute, const std::vector<unsigned short>& UShortJointAttribute, const std::vector<float>& WeightAttribute, const std::vector<std::vector<float>>& AdditionalUVAttribute,
			const std::vector<int>& IntIndices, const std::vector<unsigned char>& ByteIndices, const std::vector<unsigned short>& UShortIndices);

		const std::vector<float>& GetPositionAttribute() const;
		const std::vector<float>& GetNormalAttribute() const;
		const std::vector<float>& GetUVAttribute() const;
		const std::vector<float>& GetTangentAttribute() const;

		// MetaData.BoneIndexSizeに応じてバイト数が変わる
		const std::vector<int>& GetIntJointAttribute() const;
		const std::vector<unsigned char>& GetByteJointAttribute() const;
		const std::vector<unsigned short>& GetUShortJointAttribute() const;

		const std::vector<float>& GetWeightAttribute() const;

		const std::vector<std::vector<float>>& GetAdditionalUVAttribute() const;

		// インデックスバッファ
		// MetaData.VertexIndexSizeに応じてバイト数が変わる
		const std::vector<int>& GetIntIndices() const;
		const std::vector<unsigned char>& GetByteIndices() const;
		const std::vector<unsigned short>& GetUShortIndices() const;
	};
}
#endif