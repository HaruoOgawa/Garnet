#pragma once
#ifdef __DAWN__
#include "../../Interface/IRenderer.h"
#include <webgpu.h>
#include <wgpu.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace api { 
	class CWebGPUAPI; 
	class CWebGPUMaterial;
}

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
	private:
		// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
		bool CreateVertexBuffer(const CRendererCreateInfo& createInfo);
		bool CreateIndexBuffer(const CRendererCreateInfo& createInfo);
		bool CreateGraphicsPipeline(const CRendererCreateInfo& createInfo, api::CWebGPUMaterial* pWebGPUMat);
		
		// Helper Function ///////////////////////////////////////////////////////////////////////
		WGPUVertexFormat GetVertexFormat(int Dimension);
		bool             CreateBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize);
	public:
		CWebGPURenderer();
		virtual ~CWebGPURenderer();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo, const std::shared_ptr<graphics::CMaterial>& Material) override;
		bool Update(float SecondsTime) override;
		bool Draw(const std::shared_ptr<graphics::CMaterial>& Material) override;
	};
}
#endif