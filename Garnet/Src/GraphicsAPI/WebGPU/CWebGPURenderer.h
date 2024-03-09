#pragma once
#ifdef USE_WEBGPU
#include "../../Interface/IRenderer.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "CWebGPUAPI.h"

namespace api { 
	class CWebGPUAPI; 
	class CWebGPUMaterial;
}

namespace graphics
{
	class CRendererCreateInfo;
	enum class EIndiceType;
	enum class EDataType;

	class CWebGPURenderer : public IRenderer
	{
		// API
		api::CWebGPUAPI* m_pGraphicsAPI;

		//
		const std::string m_PassName;
		int m_InstanceCount;

		// Pipeline
		WGPURenderPipeline m_GraphicsPipeline;

		// Vertex
		int m_VertexCount;
		std::vector<WGPUBuffer> m_VertexBufferList;
		std::vector<size_t> m_VertexBufferSizeList;
		
		// Index
		EIndiceType m_IndiceType;
		WGPUBuffer m_IndexBuffer;
		size_t     m_IndicesCount;
	private:
		// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
		bool CreateVertexBuffer(const std::shared_ptr<CRendererCreateInfo>& createInfo);
		bool CreateIndexBuffer(const std::shared_ptr<CRendererCreateInfo>& createInfo);
		bool CreateGraphicsPipeline(const std::shared_ptr<CRendererCreateInfo>& createInfo, api::CWebGPUMaterial* pWebGPUMat);
		
		// Helper Function ///////////////////////////////////////////////////////////////////////
		WGPUVertexFormat GetVertexFormat(int Dimention, EDataType DataType);
		bool             CreateBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize);
		void			 SetDefaultDepthStencil(WGPUDepthStencilState& depthStencilState);
	public:
		CWebGPURenderer(api::CWebGPUAPI* pGraphicsAPI, const std::string& PassName);
		virtual ~CWebGPURenderer();

		bool Create(const std::shared_ptr<CRendererCreateInfo>& createInfo, const std::shared_ptr<graphics::CMaterial>& Material) override;
		bool Draw(const std::shared_ptr<graphics::CMaterial>& Material, int DynamicOffsetNum) override;
	};
}
#endif