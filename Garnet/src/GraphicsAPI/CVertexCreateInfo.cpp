#include "CVertexCreateInfo.h"

namespace vertex
{
	CVertexCreateInfo::CVertexCreateInfo()
	{
	}

	CVertexCreateInfo::~CVertexCreateInfo()
	{
	}

	// VertexBuffer
	void CVertexCreateInfo::SetVertices(const std::vector<std::vector<float>>& Vertices)
	{
		m_Vertices = Vertices;
	}

	const std::vector<std::vector<float>>& CVertexCreateInfo::GetVertices() const
	{
		return m_Vertices;
	}

	void CVertexCreateInfo::SetIndices(const std::vector<uint16_t>& Indices)
	{
		m_Indices = Indices;
	}

	const std::vector<uint16_t>& CVertexCreateInfo::GetIndices() const
	{
		return m_Indices;
	}

	void CVertexCreateInfo::SetAttributeDimensions(const std::vector<int>& AttributeDimensions)
	{
		m_AttributeDimensions = AttributeDimensions;
	}
	const std::vector<int>& CVertexCreateInfo::GetAttributeDimensions() const
	{
		return m_AttributeDimensions;
	}
}