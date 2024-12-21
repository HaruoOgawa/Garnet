#pragma once
#ifdef USE_VULKAN
#include <glfw3.h>
#include <glfw3native.h>

#include <vector>
#include <array>
#include <string>
#include <map>

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

		int m_InstanceCount;

		// Pipeline
		std::map<std::string, VkPipeline> m_GraphicsPipelineList;
	private:
		// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateGraphicsPipeline(const std::vector<std::string>& PassNameList, const CVulkanVertexBuffer* pVulkanVertexBuffer, api::CVulkanMaterial* pVulkanMat);
		
		void SetRuntimeGraphicsSettings(const CVulkanVertexBuffer* pVulkanVertexBuffer, api::CVulkanMaterial* pVulkanMat);

		void SetCullMode(graphics::ECullMode CullMode);

		// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		VkFormat GetVertexFormat(int Dimention, graphics::EDataType DataType);
	public:
		CVulkanRenderer(api::CVulkanAPI* pGraphicsAPI);
		virtual ~CVulkanRenderer();

		void Release();

		virtual bool Create(const std::vector<std::string>& PassNameList, const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material) override;
		virtual bool Draw(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, 
			const std::shared_ptr<graphics::CMaterial>& Material) override;
	
		virtual bool UpdateVertexBuffer(const std::vector<float>& PosAttribute, const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer) override;
	};
}
#endif