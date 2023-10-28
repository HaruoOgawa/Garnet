#pragma once
#ifdef USE_VULKAN
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
	enum class EIndiceType;
	enum class EDataType;

	class CVulkanRenderer : public IRenderer
	{
		// API
		api::CVulkanAPI* m_pGraphicsAPI;

		//
		const std::string m_PassName;
		int	m_DynamicOffsetNum;
		int m_InstanceCount;

		// Vertices/Indices
		std::vector<VkBuffer> m_VertexBufferList;
		std::vector<VkDeviceMemory> m_VertexBufferMemoryList;

		EIndiceType m_IndiceType;
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
		VkFormat GetVertexFormat(int Dimention, EDataType DataType);
	public:
		CVulkanRenderer(api::CVulkanAPI* pGraphicsAPI, const std::string& PassName);
		virtual ~CVulkanRenderer();

		void Release();

		bool Create(const std::shared_ptr<CRendererCreateInfo>& createInfo, const std::shared_ptr<graphics::CMaterial>& Material) override;
		bool Draw(const std::shared_ptr<graphics::CMaterial>& Material, int DynamicOffsetNum) override;
	};
}
#endif