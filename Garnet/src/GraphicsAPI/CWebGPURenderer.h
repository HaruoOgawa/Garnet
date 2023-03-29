#pragma once
#ifdef __DAWN__
#include "../Interface/IRenderer.h"
#include <webgpu.h>
#include <wgpu.h>
#include <vector>
#include <string>

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
		std::vector<WGPUBuffer> m_BufferList;
		std::vector<size_t> m_BufferSizeList;
		
		// Index
		WGPUBuffer m_IndexBuffer;
		size_t     m_IndexCount;

	private:
		// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
		bool CreateVertexBuffer(const CRendererCreateInfo& createInfo);
		bool CreateIndexBuffer(const CRendererCreateInfo& createInfo);
		bool CreateGraphicsPipeline(const CRendererCreateInfo& createInfo);
		
		// Helper Function ///////////////////////////////////////////////////////////////////////
		WGPUShaderModule CreateShaderModule(const std::string& shaderCode);
		WGPUVertexFormat GetVertexFormat(int Dimension);
		bool             CreateBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize);
	public:
		CWebGPURenderer();
		virtual ~CWebGPURenderer();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo) override;
		bool Update() override;
		bool Draw() override;
	};
}
#endif