#pragma once
#ifdef USE_VULKAN

#include <glfw3.h>
#include <glfw3native.h>

#include <vector>
#include <array>
#include <string>
#include <memory>

#include "../../Graphics/CIndexBuffer.h"

namespace graphics {
	enum class EIndiceType;
	enum class EDataType;
	
}

namespace api
{
	class CVulkanAPI;

	class CVulkanIndexBuffer : public graphics::CIndexBuffer
	{
		// API
		api::CVulkanAPI* m_pGraphicsAPI;

		// Indices
		graphics::EIndiceType m_IndiceType;
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexBufferMemory;
		uint32_t m_IndicesCount;
	public:
		CVulkanIndexBuffer(api::CVulkanAPI* pGraphicsAPI);
		virtual ~CVulkanIndexBuffer();

		const VkBuffer& GetIndexBuffer() const;
		uint32_t GetIndicesCount() const;

		virtual bool Create() override;

		void Bind() const;
	};
}
#endif