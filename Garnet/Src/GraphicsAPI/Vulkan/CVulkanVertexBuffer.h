#pragma once
#ifdef USE_VULKAN
#include <glfw3.h>
#include <glfw3native.h>

#include <vector>
#include <array>
#include <string>
#include <memory>

#include "../../Graphics/CVertexBuffer.h"

namespace api
{
	class CVulkanAPI;

	enum class EIndiceType;
	enum class EDataType;

	class CVulkanVertexBuffer : public graphics::CVertexBuffer
	{
		// API
		api::CVulkanAPI* m_pGraphicsAPI;

		// Vertices
		std::vector<VkBuffer> m_VertexBufferList;
		std::vector<VkDeviceMemory> m_VertexBufferMemoryList;

	public:
		CVulkanVertexBuffer(api::CVulkanAPI* pGraphicsAPI);
		virtual ~CVulkanVertexBuffer();

		const std::vector<VkBuffer>& GetVertexBufferList() const;
		const std::vector<VkDeviceMemory> GetVertexBufferMemoryList() const;

		virtual bool Create() override;
	};
}
#endif