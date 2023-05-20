#pragma once
#ifndef __DAWN__
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <glfw3.h>
#include <glfw3native.h>

#include <vector>
#include <array>
#include <string>

#include "../../Interface/IRenderer.h"

// はぇー時間を管理するstd標準ライブラリ
#include <chrono>

namespace api { 
	class CVulkanAPI; 
	class CVulkanMaterial;
}

namespace renderer
{
	class CRendererCreateInfo;

	class CVulkanRenderer : public IRenderer
	{
		int	m_DynamicOffsetNum;

		// API
		api::CVulkanAPI* m_pGraphicsAPI;

		// Vertices/Indices
		std::vector<VkBuffer> m_VertexBufferList;
		std::vector<VkDeviceMemory> m_VertexBufferMemoryList;
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexBufferMemory;
		uint32_t m_IndicesCount;

		// Pipeline
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;
	private:
		// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateVertexBuffer(const std::shared_ptr<CRendererCreateInfo>& createInfo);
		bool CreateIndexBuffer(const std::shared_ptr<CRendererCreateInfo>& createInfo);
		bool CreateGraphicsPipeline(const std::shared_ptr<CRendererCreateInfo>& createInfo, api::CVulkanMaterial* pVulkanMat);

		// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		VkFormat GetVertexFormat(int Dimention);
	public:
		CVulkanRenderer();
		virtual ~CVulkanRenderer();

		void Release();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CRendererCreateInfo>& createInfo, const std::shared_ptr<graphics::CMaterial>& Material) override;
		bool Draw(const std::shared_ptr<graphics::CMaterial>& Material, int DynamicOffsetNum) override;
	};
}
#endif