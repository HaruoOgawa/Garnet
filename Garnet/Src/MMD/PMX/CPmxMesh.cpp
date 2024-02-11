#ifdef USE_MMD
#include "CPmxMesh.h"

namespace mmd
{
	CPmxMesh::CPmxMesh(const std::vector<float>& PositionAttribute, const std::vector<float>& NormalAttribute, const std::vector<float>& UVAttribute, const std::vector<float>& TangentAttribute, const std::vector<unsigned int>& UIntBoneAttribute,
		const std::vector<unsigned char>& ByteBoneAttribute, const std::vector<unsigned short>& UShortBoneAttribute, const std::vector<float>& WeightAttribute, const std::vector<std::vector<float>>& AdditionalUVAttribute,
		const std::vector<unsigned int>& UIntIndices, const std::vector<unsigned char>& ByteIndices, const std::vector<unsigned short>& UShortIndices) :
		m_PositionAttribute(PositionAttribute),
		m_NormalAttribute(NormalAttribute),
		m_UVAttribute(UVAttribute),
		m_TangentAttribute(TangentAttribute),
		m_UIntBoneAttribute(UIntBoneAttribute),
		m_ByteBoneAttribute(ByteBoneAttribute),
		m_UShortBoneAttribute(UShortBoneAttribute),
		m_WeightAttribute(WeightAttribute),
		m_AdditionalUVAttribute(AdditionalUVAttribute),
		m_UIntIndices(UIntIndices),
		m_ByteIndices(ByteIndices),
		m_UShortIndices(UShortIndices)
	{
	}

	const std::vector<float>& CPmxMesh::GetPositionAttribute() const
	{
		return m_PositionAttribute;
	}

	const std::vector<float>& CPmxMesh::GetNormalAttribute() const
	{
		return m_NormalAttribute;
	}

	const std::vector<float>& CPmxMesh::GetUVAttribute() const
	{
		return m_UVAttribute;
	}

	const std::vector<float>& CPmxMesh::GetTangentAttribute() const
	{
		return m_TangentAttribute;
	}

	// MetaData.BoneIndexSizeに応じてバイト数が変わる
	const std::vector<unsigned int>& CPmxMesh::GetUIntBoneAttribute() const
	{
		return m_UIntBoneAttribute;
	}

	const std::vector<unsigned char>& CPmxMesh::GetByteBoneAttribute() const
	{
		return m_ByteBoneAttribute;
	}

	const std::vector<unsigned short>& CPmxMesh::GetUShortBoneAttribute() const
	{
		return m_UShortBoneAttribute;
	}

	const std::vector<float>& CPmxMesh::GetWeightAttribute() const
	{
		return m_WeightAttribute;
	}

	const std::vector<std::vector<float>>& CPmxMesh::GetAdditionalUVAttribute() const
	{
		return m_AdditionalUVAttribute;
	}

	// インデックスバッファ
	// MetaData.VertexIndexSizeに応じてバイト数が変わる
	const std::vector<unsigned int>& CPmxMesh::GetUIntIndices() const
	{
		return m_UIntIndices;
	}

	const std::vector<unsigned char>& CPmxMesh::GetByteIndices() const
	{
		return m_ByteIndices;
	}

	const std::vector<unsigned short>& CPmxMesh::GetUShortIndices() const
	{
		return m_UShortIndices;
	}
}
#endif