#pragma once

#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

#include <glm/glm.hpp>

#include <vector>
#include <array>

#include "../Interface/IRenderer.h"

namespace api { class CVulkanAPI; }

namespace renderer
{
	class CRendererCreateInfo;

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription GetBindingDescription() {
			// 頂点バッファのバインドに関する説明,設定
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0; // バインドする頂点バッファのインデックス(?)違う形式で頂点バッファを用意するときに使用する？
			bindingDescription.stride = sizeof(Vertex); // 頂点バッファ内の要素一つあたりのサイズ。次の要素までのバイト数
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // よくわからぬ。各頂点の後、次のデータ エントリに移動します。らしい

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDestriptions() {
			// アトリビュート(頂点データ)の設定
			// 配列の要素数は頂点バッファのアトリビュートの数だけ
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

			// posotionアトリビュートの設定
			attributeDescriptions[0].binding = 0; // BindingDescriptionの内どのバインド設定を使用するかのインデックス
			attributeDescriptions[0].location = 0; // Shaderのlayout(location = 0)に設定すｒ数値
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // データ型. SFLOAT --> Signed Float
			attributeDescriptions[0].offset = offsetof(Vertex, pos); // データオフセット

			// colorアトリビュートの設定
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			// texCoordアトリビュートの設定
			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}
	};


	class CVulkanRenderer : public IRenderer
	{
		// Uniform Buffer Object
		VkDescriptorSetLayout m_DescriptorSetLayout;
		std::vector<VkBuffer> m_UniformBuffers;
		std::vector<VkDeviceMemory> m_UniformBuffersMemory;
		std::vector<void*> m_UniformBuffersMapped;
		VkDescriptorPool m_DescriptorPool;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		// Pipeline
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;

	private:
		// 初期化関数 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateDescriptorSetLayout(api::CVulkanAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo);
		bool CreateGraphicsPipeline(api::CVulkanAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo);

		// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Shader
		bool CreateShaderModule(VkShaderModule& shaderModule, api::CVulkanAPI* pGraphicsAPI, const std::vector<unsigned char>& code);
	public:
		CVulkanRenderer();
		virtual ~CVulkanRenderer();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo) override;
		bool Draw() override;
	};
}