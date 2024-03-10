#include "CVertexBuffer.h"

namespace graphics
{
	CVertexBuffer::CVertexBuffer():
		m_InstanceCount(1)
	{
	}

	CVertexBuffer::~CVertexBuffer()
	{
		Release();
	}

	void CVertexBuffer::Release()
	{
		m_Vertices.clear();
		m_AttributeDimensions.clear();
		m_AttribDataTypes.clear();
		m_AttribByteStrides.clear();
	}

	void CVertexBuffer::SetVertices(const std::vector<std::vector<float>>& Vertices)
	{
		m_Vertices = Vertices;
	}

	const std::vector<std::vector<float>>& CVertexBuffer::GetVertices() const
	{
		return m_Vertices;
	}

	void CVertexBuffer::SetAttributeDimensions(const std::vector<int>& AttributeDimensions)
	{
		m_AttributeDimensions = AttributeDimensions;
	}
	const std::vector<int>& CVertexBuffer::GetAttributeDimensions() const
	{
		return m_AttributeDimensions;
	}

	void CVertexBuffer::SetAttribDataTypes(const std::vector<EDataType>& AttribDataTypes)
	{
		m_AttribDataTypes = AttribDataTypes;
	}

	const std::vector<EDataType>& CVertexBuffer::GetAttribDataTypes() const
	{
		return m_AttribDataTypes;
	}

	void CVertexBuffer::SetAttribByteStrides(const std::vector<int>& AttribByteStrides)
	{
		m_AttribByteStrides = AttribByteStrides;
	}

	const std::vector<int>& CVertexBuffer::GetAttribByteStrides() const
	{
		return m_AttribByteStrides;
	}

#ifdef USE_GPGPU
	void CVertexBuffer::SetInstanceDrawCount(int InstanceCount)
	{
		m_InstanceCount = InstanceCount;
	}
#endif // USE_GPGPU

	int CVertexBuffer::GetInstanceCount() const
	{
		return m_InstanceCount;
	}
}