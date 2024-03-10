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
#include "../../Graphics/ECullMode.h"

// はぇー時間を管理するstd標準ライブラリ
#include <chrono>

namespace graphics
{
	class CVertexBuffer;
	class CIndexBuffer;
	enum class EDataType;
	enum class EIndiceType;
}

namespace api
{
	class CVulkanAPI;
	class CVulkanMaterial;

	class CVulkanRenderer : public graphics::IRenderer
	{
		// API
		api::CVulkanAPI* m_pGraphicsAPI;

		//
		const std::string m_PassName;
		int	m_DynamicOffsetNum;
		int m_InstanceCount;

		// Pipeline
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;
	private:
		// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateGraphicsPipeline(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, api::CVulkanMaterial* pVulkanMat);

		void SetCullMode(graphics::ECullMode CullMode);

		// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		VkFormat GetVertexFormat(int Dimention, graphics::EDataType DataType);
	public:
		CVulkanRenderer(api::CVulkanAPI* pGraphicsAPI, const std::string& PassName);
		virtual ~CVulkanRenderer();

		void Release();

		virtual bool Create(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material) override;
		virtual bool Draw(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material, int DynamicOffsetNum) override;
	
		virtual bool UpdateVertexBuffer(const std::vector<float>& PosAttribute, const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer) override;
	};
}
#endif