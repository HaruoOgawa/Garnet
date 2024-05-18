#ifdef USE_VULKAN
#include "CVulkanIndexBuffer.h"
#include "CVulkanAPI.h"

namespace api
{
	CVulkanIndexBuffer::CVulkanIndexBuffer(api::CVulkanAPI* pGraphicsAPI):
		CIndexBuffer(),
		m_pGraphicsAPI(pGraphicsAPI),
		m_IndiceType(graphics::EIndiceType::UNSIGNED_SHORT),
		m_IndexBuffer(nullptr),
		m_IndexBufferMemory(nullptr),
		m_IndicesCount(0)
	{
	}

	CVulkanIndexBuffer::~CVulkanIndexBuffer()
	{
		// インデックスバッファの破棄
		if (m_IndexBuffer)
		{
			vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), m_IndexBuffer, nullptr);
			m_IndexBuffer = nullptr;
		}

		// インデックスバッファ用に確保したメモリ領域を破棄
		if (m_IndexBufferMemory)
		{
			vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), m_IndexBufferMemory, nullptr);
			m_IndexBufferMemory = nullptr;
		}
	}

	const VkBuffer& CVulkanIndexBuffer::GetIndexBuffer() const
	{
		return m_IndexBuffer;
	}

	void CVulkanIndexBuffer::Bind() const
	{
		if (GetIndiceType() == graphics::EIndiceType::UNSIGNED_SHORT)
		{
			vkCmdBindIndexBuffer(m_pGraphicsAPI->GetCurrentCommandBuffer(), GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);
		}
		else if (GetIndiceType() == graphics::EIndiceType::UNSIGNED_INT)
		{
			vkCmdBindIndexBuffer(m_pGraphicsAPI->GetCurrentCommandBuffer(), GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	uint32_t CVulkanIndexBuffer::GetIndicesCount() const
	{
		return m_IndicesCount;
	}

	bool CVulkanIndexBuffer::Create()
	{
		m_IndiceType = GetIndiceType();

		VkDeviceSize bufferSize = 0;

		if (m_IndiceType == graphics::EIndiceType::UNSIGNED_SHORT)
		{
			m_IndicesCount = static_cast<uint32_t>(GetIndices().size());

			bufferSize = sizeof(GetIndices()[0]) * GetIndices().size();
		}
		else if (m_IndiceType == graphics::EIndiceType::UNSIGNED_INT)
		{
			m_IndicesCount = static_cast<uint32_t>(GetUINTIndices().size());

			bufferSize = sizeof(GetUINTIndices()[0]) * GetUINTIndices().size();
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);

		if (m_IndiceType == graphics::EIndiceType::UNSIGNED_SHORT)
		{
			memcpy(data, GetIndices().data(), (size_t)bufferSize);
		}
		else if (m_IndiceType == graphics::EIndiceType::UNSIGNED_INT)
		{
			memcpy(data, GetUINTIndices().data(), (size_t)bufferSize);
		}

		vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory);

		m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_IndexBuffer, m_IndexBufferMemory);

		m_pGraphicsAPI->CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

		vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, nullptr);

		return true;
	}
}
#endif