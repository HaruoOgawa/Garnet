#include "CVulkanVertex.h"
#include "CVulkanAPI.h"
#include "../CVertexCreateInfo.h"

namespace vertex
{
	CVulkanVertex::CVulkanVertex():
		m_IndicesCount(0)
	{
	}

	CVulkanVertex::~CVulkanVertex()
	{
	}

	void CVulkanVertex::Release()
	{
		// インデックスバッファの破棄
		vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), m_IndexBuffer, nullptr);

		// インデックスバッファ用に確保したメモリ領域を破棄
		vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), m_IndexBufferMemory, nullptr);

		// 頂点バッファの破棄
		for (auto& Buffer : m_VertexBufferList)
		{
			vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), Buffer, nullptr);
		}

		// 頂点バッファ用に確保したメモリ領域を破棄
		for (auto& Memory : m_VertexBufferMemoryList)
		{
			vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), Memory, nullptr);
		}
	}

	bool CVulkanVertex::Create(api::IGraphicsAPI* pGraphicsAPI, const CVertexCreateInfo& createInfo)
	{
		m_pGraphicsAPI = static_cast<api::CVulkanAPI*>(pGraphicsAPI);

		if (!CreateVertexBuffer(createInfo)) return false; // 頂点バッファを作成
		if (!CreateIndexBuffer(createInfo)) return false; // インデックスバッファを作成

		return true;
	}

	// Vulkanメインロジック
	bool CVulkanVertex::CreateVertexBuffer(const CVertexCreateInfo& createInfo)
	{
		// 頂点バッファオブジェクトの生成
		for (const auto& VertexData : createInfo.GetVertices())
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
	bool CVulkanVertex::CreateIndexBuffer(const CVertexCreateInfo& createInfo)
	{
		VkDeviceSize bufferSize = sizeof(createInfo.GetIndices()[0]) * createInfo.GetIndices().size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, createInfo.GetIndices().data(), (size_t)bufferSize);
		vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory);

		m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_IndexBuffer, m_IndexBufferMemory);

		m_pGraphicsAPI->CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

		vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, nullptr);

		m_IndicesCount = static_cast<uint32_t>(createInfo.GetIndices().size());

		return true;
	}
}