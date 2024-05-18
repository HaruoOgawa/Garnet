#pragma once
#ifdef USE_VULKAN
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
	class CVulkanVertexBuffer;
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
		VkPipeline m_GraphicsPipeline;
	private:
		// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateGraphicsPipeline(const CVulkanVertexBuffer* pVulkanVertexBuffer, api::CVulkanMaterial* pVulkanMat);
		
		void SetRuntimeGraphicsSettings(const CVulkanVertexBuffer* pVulkanVertexBuffer, api::CVulkanMaterial* pVulkanMat);

		void SetCullMode(graphics::ECullMode CullMode);

		// Vulkan Extensions /////////////////////////////////////////////////////////////////////
		void SetVertexInputEXT(uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription2EXT* pVertexBindingDescriptions, 
			uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions);

		void SetPrimitiveTopologyEXT(VkPrimitiveTopology primitiveTopology);

		void SetPrimitiveRestartEnableEXT(VkBool32 primitiveRestartEnable);

		void SetViewportWithCountEXT(uint32_t viewportCount, const VkViewport* pViewports);

		void SetScissorWithCountEXT(uint32_t scissorCount, const VkRect2D* pScissors);

		void SetCullModeEXT(VkCullModeFlags cullMode);

		void SetFrontFaceEXT(VkFrontFace frontFace);

		void SetRasterizerDiscardEnableEXT(VkBool32 rasterizerDiscardEnable);

		void SetPolygonModeEXT(VkPolygonMode polygonMode);

		void SetRasterizationSamplesEXT(VkSampleCountFlagBits rasterizationSamples);

		void SetAlphaToCoverageEnableEXT(VkBool32 alphaToCoverageEnable);

		void SetDepthTestEnableEXT(VkBool32 depthTestEnable);

		void SetDepthWriteEnableEXT(VkBool32 depthWriteEnable);

		void SetDepthCompareOpEXT(VkCompareOp depthCompareOp);

		void SetDepthBiasEnableEXT(VkBool32 depthBiasEnable);

		void SetStencilTestEnableEXT(VkBool32 stencilTestEnable);

		void SetSampleMaskEXT(VkSampleCountFlagBits samples, const VkSampleMask* pSampleMask);

		void SetColorBlendEnableEXT(uint32_t firstAttachment, uint32_t attachmentCount, const VkBool32* pColorBlendEnables);

		void SetColorWriteMaskEXT(uint32_t firstAttachment, uint32_t attachmentCount, const VkColorComponentFlags* pColorWriteMasks);

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