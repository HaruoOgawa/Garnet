#ifdef USE_WEBGPU
#include "CWebGPUIndexBuffer.h"

namespace api
{
	CWebGPUIndexBuffer::CWebGPUIndexBuffer(api::CWebGPUAPI* pGraphicsAPI):
		m_pGraphicsAPI(pGraphicsAPI),
		m_IndiceType(graphics::EIndiceType::UNSIGNED_SHORT),
		m_IndexBuffer(nullptr),
		m_IndicesCount(0)
	{
	}

	CWebGPUIndexBuffer::~CWebGPUIndexBuffer()
	{
		if (m_IndexBuffer)
		{
			wgpuBufferDestroy(m_IndexBuffer);
			m_IndexBuffer = nullptr;
		}
	}

	const WGPUBuffer& CWebGPUIndexBuffer::GetIndexBuffer() const
	{
		return m_IndexBuffer;
	}

	size_t CWebGPUIndexBuffer::GetIndicesCount() const
	{
		return m_IndicesCount;
	}

	bool CWebGPUIndexBuffer::Create()
	{
		m_IndiceType = GetIndiceType();

		if (m_IndiceType == graphics::EIndiceType::UNSIGNED_SHORT)
		{
			m_IndicesCount = GetIndices().size();

			if (!m_pGraphicsAPI->CreateBuffer(m_IndexBuffer, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index, &GetIndices()[0], m_IndicesCount * sizeof(uint16_t))) return false;
		}
		else if (m_IndiceType == graphics::EIndiceType::UNSIGNED_INT)
		{
			m_IndicesCount = GetUINTIndices().size();

			if (!m_pGraphicsAPI->CreateBuffer(m_IndexBuffer, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index, &GetUINTIndices()[0], m_IndicesCount * sizeof(uint32_t))) return false;
		}

		return true;
	}
}
#endif