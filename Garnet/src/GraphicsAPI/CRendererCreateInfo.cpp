#include "CRendererCreateInfo.h"

namespace renderer
{
	CRendererCreateInfo::CRendererCreateInfo()
	{
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

	void CRendererCreateInfo::SetIndices(const std::vector<uint16_t>& Indices)
	{
		m_Indices = Indices;
	}

	const std::vector<uint16_t>& CRendererCreateInfo::GetIndices() const
	{
		return m_Indices;
	}

	void CRendererCreateInfo::SetAttributeDimensions(const std::vector<int>& AttributeDimensions)
	{
		m_AttributeDimensions = AttributeDimensions;
	}
	const std::vector<int>& CRendererCreateInfo::GetAttributeDimensions() const
	{
		return m_AttributeDimensions;
	}
}