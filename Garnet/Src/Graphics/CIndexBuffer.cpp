#include "CIndexBuffer.h"

namespace graphics
{
	CIndexBuffer::CIndexBuffer():
		m_IndiceType(EIndiceType::UNSIGNED_SHORT)
	{
	}

	CIndexBuffer::~CIndexBuffer()
	{
		Release();
	}

	void CIndexBuffer::Release()
	{
		m_Indices.clear();
		m_UINTIndices.clear();
	}

	void CIndexBuffer::SetIndices(const std::vector<unsigned short>& Indices)
	{
		m_IndiceType = EIndiceType::UNSIGNED_SHORT;

		m_Indices = Indices;
	}

	const std::vector<unsigned short>& CIndexBuffer::GetIndices() const
	{
		return m_Indices;
	}

	void CIndexBuffer::SetUINTIndices(const std::vector<unsigned int>& Indices)
	{
		m_IndiceType = EIndiceType::UNSIGNED_INT;

		m_UINTIndices = Indices;
	}

	const std::vector<unsigned int>& CIndexBuffer::GetUINTIndices() const
	{
		return m_UINTIndices;
	}

	EIndiceType CIndexBuffer::GetIndiceType() const
	{
		return m_IndiceType;
	}
}