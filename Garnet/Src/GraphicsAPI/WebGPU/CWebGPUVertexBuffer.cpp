#ifdef USE_WEBGPU
#include "CWebGPUVertexBuffer.h"

namespace api
{
	CWebGPUVertexBuffer::CWebGPUVertexBuffer(api::CWebGPUAPI* pGraphicsAPI):
		m_pGraphicsAPI(pGraphicsAPI),
		m_VertexCount(0)
	{
	}

	CWebGPUVertexBuffer::~CWebGPUVertexBuffer()
	{
		for (auto& Buffer : m_VertexBufferList)
		{
			if (Buffer)
			{
				wgpuBufferDestroy(Buffer);
			}
		}
	}

	const std::vector<WGPUBuffer>& CWebGPUVertexBuffer::GetVertexBufferList() const
	{
		return m_VertexBufferList;
	}

	const std::vector<size_t>& CWebGPUVertexBuffer::GetVertexBufferSizeList() const
	{
		return m_VertexBufferSizeList;
	}

	bool CWebGPUVertexBuffer::Create()
	{
		// 頂点バッファオブジェクトの生成
		for (const auto& Data : GetVertices())
		{
			// WGPUBufferUsage_CopyDst はCPUからGPUへメモリをコピーすることを指定する
			// 反対にGPUからCPUへ読み戻したい場合はWGPUBufferUsage_CopySrcも指定する

			WGPUBuffer Buffer;
			if (!m_pGraphicsAPI->CreateBuffer(Buffer, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex, &Data[0], Data.size() * sizeof(float))) return false;

			// バッファを保存
			m_VertexBufferList.push_back(Buffer);
			m_VertexBufferSizeList.push_back(Data.size());
		}

		// 頂点数
		m_VertexCount = static_cast<int>(GetVertices()[0].size() / GetAttributeDimensions()[0]);

		return true;
	}
}
#endif