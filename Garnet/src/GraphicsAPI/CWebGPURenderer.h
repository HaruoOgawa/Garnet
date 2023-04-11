#pragma once
#ifdef __DAWN__
#include "../Interface/IRenderer.h"
#include <webgpu.h>
#include <wgpu.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace api { class CWebGPUAPI; }

namespace renderer
{
	class CRendererCreateInfo;

	class CWebGPURenderer : public IRenderer
	{
		// API
		api::CWebGPUAPI* m_pGraphicsAPI;

		// Pipeline
		WGPURenderPipeline m_GraphicsPipeline;

		// Vertex
		int m_VertexCount;
		std::vector<WGPUBuffer> m_VertexBufferList;
		std::vector<size_t> m_VertexBufferSizeList;
		
		// Index
		WGPUBuffer m_IndexBuffer;
		size_t     m_IndexCount;

		// Uniform
		WGPUBuffer m_UniformBuffer;
		size_t	   m_UniformCount;

		// BindGroup
		WGPUBindGroupLayout m_BindGroupLayout;
		WGPUBindGroup m_BindGroup;

	private:
		// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
		bool CreateVertexBuffer(const CRendererCreateInfo& createInfo);
		bool CreateIndexBuffer(const CRendererCreateInfo& createInfo);
		bool CreateUniformBuffer(const CRendererCreateInfo& createInfo);
		bool CreateBindGroup(const CRendererCreateInfo& createInfo);
		bool CreateGraphicsPipeline(const CRendererCreateInfo& createInfo);
		
		// Helper Function ///////////////////////////////////////////////////////////////////////
		WGPUShaderModule CreateShaderModule(const std::string& shaderCode);
		WGPUVertexFormat GetVertexFormat(int Dimension);
		bool             CreateBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize);
		void			 InitDefalutBindGroupLayoutEntry(WGPUBindGroupLayoutEntry& bindingLayout);
	public:
		CWebGPURenderer();
		virtual ~CWebGPURenderer();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo) override;
		bool Update(float SecondsTime) override;
		bool Draw() override;
	};
}
#endif