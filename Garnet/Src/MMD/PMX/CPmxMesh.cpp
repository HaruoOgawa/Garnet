#ifdef USE_MMD
#include "CPmxMesh.h"

namespace mmd
{
	CPmxMesh::CPmxMesh(const std::vector<float>& PositionAttribute, const std::vector<float>& NormalAttribute, const std::vector<float>& UVAttribute, const std::vector<float>& TangentAttribute, const std::vector<unsigned int>& UIntJointAttribute,
		const std::vector<unsigned char>& ByteJointAttribute, const std::vector<unsigned short>& UShortJointAttribute, const std::vector<float>& WeightAttribute, const std::vector<std::vector<float>>& AdditionalUVAttribute,
		const std::vector<unsigned int>& UIntIndices, const std::vector<unsigned char>& ByteIndices, const std::vector<unsigned short>& UShortIndices) :
		m_PositionAttribute(PositionAttribute),
		m_NormalAttribute(NormalAttribute),
		m_UVAttribute(UVAttribute),
		m_TangentAttribute(TangentAttribute),
		m_UIntJointAttribute(UIntJointAttribute),
		m_ByteJointAttribute(ByteJointAttribute),
		m_UShortJointAttribute(UShortJointAttribute),
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
	const std::vector<unsigned int>& CPmxMesh::GetUIntJointAttribute() const
	{
		return m_UIntJointAttribute;
	}

	const std::vector<unsigned char>& CPmxMesh::GetByteJointAttribute() const
	{
		return m_ByteJointAttribute;
	}

	const std::vector<unsigned short>& CPmxMesh::GetUShortJointAttribute() const
	{
		return m_UShortJointAttribute;
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