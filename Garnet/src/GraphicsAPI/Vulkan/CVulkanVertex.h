#pragma once
#ifndef __DAWN__
#include "../../Interface/IVertex.h"
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <vector>

namespace api { class CVulkanAPI; }

namespace vertex
{
	class CVertexCreateInfo;

	class CVulkanVertex : public IVertex
	{
		api::CVulkanAPI* m_pGraphicsAPI;

		// Vertices/Indices
		std::vector<VkBuffer> m_VertexBufferList;
		std::vector<VkDeviceMemory> m_VertexBufferMemoryList;
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexBufferMemory;
		uint32_t m_IndicesCount;
	private:
		void Release();

		bool CreateVertexBuffer(const CVertexCreateInfo& createInfo);
		bool CreateIndexBuffer(const CVertexCreateInfo& createInfo);
	public:
		CVulkanVertex();
		virtual ~CVulkanVertex();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const CVertexCreateInfo& createInfo) override;
	};
}
#endif