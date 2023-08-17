#include "CRendererCreateInfo.h"

namespace renderer
{
	CRendererCreateInfo::CRendererCreateInfo():
		m_InstanceCount(1)
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
		m_Indices = Indices;
	}

	const std::vector<unsigned short>& CRendererCreateInfo::GetIndices() const
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