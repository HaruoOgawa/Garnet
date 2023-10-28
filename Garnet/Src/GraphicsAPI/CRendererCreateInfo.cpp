#include "CRendererCreateInfo.h"

namespace renderer
{
	CRendererCreateInfo::CRendererCreateInfo():
		m_InstanceCount(1),
		m_IndiceType(EIndiceType::UNSIGNED_SHORT)
	{
	}

	CRendererCreateInfo::~CRendererCreateInfo()
	{
		m_Vertices.clear();
		m_Indices.clear();
		m_AttributeDimensions.clear();
	}

	// VertexBuffer
	void CRendererCreateInfo::SetVertices(const std::vector<std::vector<float>>& Vertices)
	{
		m_Vertices = Vertices;
	}

	const std::vector<std::vector<float>>& CRendererCreateInfo::GetVertices() const
	{
		return m_Vertices;
	}

	void CRendererCreateInfo::SetIndices(const std::vector<unsigned short>& Indices)
	{
		m_IndiceType = EIndiceType::UNSIGNED_SHORT;

		m_Indices = Indices;
	}

	const std::vector<unsigned short>& CRendererCreateInfo::GetIndices() const
	{
		return m_Indices;
	}

	void CRendererCreateInfo::SetUINTIndices(const std::vector<unsigned int>& Indices)
	{
		m_IndiceType = EIndiceType::UNSIGNED_INT;

		m_UINTIndices = Indices;
	}

	const std::vector<unsigned int>& CRendererCreateInfo::GetUINTIndices() const
	{
		return m_UINTIndices;
	}

	EIndiceType CRendererCreateInfo::GetIndiceType() const
	{
		return m_IndiceType;
	}

	void CRendererCreateInfo::SetAttributeDimensions(const std::vector<int>& AttributeDimensions)
	{
		m_AttributeDimensions = AttributeDimensions;
	}
	const std::vector<int>& CRendererCreateInfo::GetAttributeDimensions() const
	{
		return m_AttributeDimensions;
	}

	void CRendererCreateInfo::SetAttribDataTypes(const std::vector<EDataType>& AttribDataTypes)
	{
		m_AttribDataTypes = AttribDataTypes;
	}

	const std::vector<EDataType>& CRendererCreateInfo::GetAttribDataTypes() const
	{
		return m_AttribDataTypes;
	}

	void CRendererCreateInfo::SetAttribByteStrides(const std::vector<int>& AttribByteStrides)
	{
		m_AttribByteStrides = AttribByteStrides;
	}

	const std::vector<int>& CRendererCreateInfo::GetAttribByteStrides()
	{
		return m_AttribByteStrides;
	}

#ifdef USE_GPGPU
	void CRendererCreateInfo::SetInstanceDrawCount(int InstanceCount)
	{
		m_InstanceCount = InstanceCount;
	}
#endif // USE_GPGPU

	int CRendererCreateInfo::GetInstanceCount() const
	{
		return m_InstanceCount;
	}
}