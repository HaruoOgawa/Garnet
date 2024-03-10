#ifdef USE_VULKAN
#include "CVulkanVertexBuffer.h"
#include "CVulkanAPI.h"

namespace api
{
	CVulkanVertexBuffer::CVulkanVertexBuffer(api::CVulkanAPI* pGraphicsAPI):
		CVertexBuffer(),
		m_pGraphicsAPI(pGraphicsAPI)
	{
	}

	CVulkanVertexBuffer::~CVulkanVertexBuffer()
	{
		// 頂点バッファの破棄
		for (auto& Buffer : m_VertexBufferList)
		{
			if (Buffer)
			{
				vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), Buffer, nullptr);
			}
		}
		m_VertexBufferList.clear();

		// 頂点バッファ用に確保したメモリ領域を破棄
		for (auto& Memory : m_VertexBufferMemoryList)
		{
			if (Memory)
			{
				vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), Memory, nullptr);
			}
		}
		m_VertexBufferMemoryList.clear();
	}

	const std::vector<VkBuffer>& CVulkanVertexBuffer::GetVertexBufferList() const
	{
		return m_VertexBufferList;
	}

	const std::vector<VkDeviceMemory> CVulkanVertexBuffer::GetVertexBufferMemoryList() const
	{
		return m_VertexBufferMemoryList;
	}

	bool CVulkanVertexBuffer::Create()
	{
		// 頂点バッファオブジェクトの生成
		for (const auto& VertexData : GetVertices())
		{
			//
			VkDeviceSize bufferSize = sizeof(VertexData[0]) * VertexData.size();

			// ステージングバッファの作成
			// ステージングバッファは頂点データ配列からデータをアップロードするのに使用するCPUアクセス可なバッファ
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				stagingBuffer, stagingBufferMemory);

			// 頂点データを渡すためのメモリのポインターを取得
			void* data;
			vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);

			// 取得したポインタにデータをコピーする
			std::memcpy(data, VertexData.data(), (size_t)bufferSize);

			// マップを解除する。たぶんマップというのはCPUからGPUへデータを渡すために一時的に確保される入口みたいなものかな？
			// 渡し終わったのでポインタという名の通路・入口を破棄したみたいな
			vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory);

			// 最終的に頂点バッファを保持するのに使用するバッファを作成
			VkBuffer Buffer;
			VkDeviceMemory BufferMemory;

			m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				Buffer, BufferMemory);

			// バッファをコピー
			m_pGraphicsAPI->CopyBuffer(stagingBuffer, Buffer, bufferSize);

			// 不要なリソースを破棄
			vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), stagingBuffer, nullptr);
			vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, nullptr);

			// バッファを保存
			m_VertexBufferList.push_back(Buffer);
			m_VertexBufferMemoryList.push_back(BufferMemory);
		}

		return true;
	}
}
#endif