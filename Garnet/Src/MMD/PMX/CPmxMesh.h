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
		std::vector<unsigned int> m_UIntBoneAttribute;
		std::vector<unsigned char> m_ByteBoneAttribute;
		std::vector<unsigned short> m_UShortBoneAttribute;

		std::vector<float> m_WeightAttribute;

		std::vector<std::vector<float>> m_AdditionalUVAttribute;

		// インデックスバッファ
		// MetaData.VertexIndexSizeに応じてバイト数が変わる
		std::vector<unsigned int> m_UIntIndices;
		std::vector<unsigned char> m_ByteIndices;
		std::vector<unsigned short> m_UShortIndices;

	public:
		CPmxMesh(const std::vector<float>& PositionAttribute, const std::vector<float>& NormalAttribute, const std::vector<float>& UVAttribute, const std::vector<float>& TangentAttribute, const std::vector<unsigned int>& UIntBoneAttribute, 
			const std::vector<unsigned char>& ByteBoneAttribute, const std::vector<unsigned short>& UShortBoneAttribute, const std::vector<float>& WeightAttribute, const std::vector<std::vector<float>>& AdditionalUVAttribute,
			const std::vector<unsigned int>& UIntIndices, const std::vector<unsigned char>& ByteIndices, const std::vector<unsigned short>& UShortIndices);

		const std::vector<float>& GetPositionAttribute() const;
		const std::vector<float>& GetNormalAttribute() const;
		const std::vector<float>& GetUVAttribute() const;
		const std::vector<float>& GetTangentAttribute() const;

		// MetaData.BoneIndexSizeに応じてバイト数が変わる
		const std::vector<unsigned int>& GetUIntBoneAttribute() const;
		const std::vector<unsigned char>& GetByteBoneAttribute() const;
		const std::vector<unsigned short>& GetUShortBoneAttribute() const;

		const std::vector<float>& GetWeightAttribute() const;

		const std::vector<std::vector<float>>& GetAdditionalUVAttribute() const;

		// インデックスバッファ
		// MetaData.VertexIndexSizeに応じてバイト数が変わる
		const std::vector<unsigned int>& GetUIntIndices() const;
		const std::vector<unsigned char>& GetByteIndices() const;
		const std::vector<unsigned short>& GetUShortIndices() const;
	};
}
#endif