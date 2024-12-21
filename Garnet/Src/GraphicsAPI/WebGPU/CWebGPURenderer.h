#pragma once
#ifdef USE_WEBGPU
#include "../../Interface/IRenderer.h"
#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "CWebGPUAPI.h"

namespace graphics
{
	class CVertexBuffer;
	class CIndexBuffer;
	enum class EDataType;
	enum class EIndiceType;
}

namespace api
{
	class CWebGPUAPI;
	class CWebGPUMaterial;

	class CWebGPURenderer : public graphics::IRenderer
	{
		// API
		api::CWebGPUAPI* m_pGraphicsAPI;

		int m_InstanceCount;

		// Pipeline
		std::map<std::string, WGPURenderPipeline> m_GraphicsPipelineList;
	private:
		// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
		bool CreateGraphicsPipeline(const std::vector<std::string>& PassNameList, const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, api::CWebGPUMaterial* pWebGPUMat);
		
		// Helper Function ///////////////////////////////////////////////////////////////////////
		WGPUVertexFormat GetVertexFormat(int Dimention, graphics::EDataType DataType);
		void			 SetDefaultDepthStencil(WGPUDepthStencilState& depthStencilState);
	public:
		CWebGPURenderer(api::CWebGPUAPI* pGraphicsAPI);
		virtual ~CWebGPURenderer();

		virtual bool Create(const std::vector<std::string>& PassNameList, const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material) override;
		virtual bool Draw(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, 
			const std::shared_ptr<graphics::CMaterial>& Material) override;

		virtual bool UpdateVertexBuffer(const std::vector<float>& PosAttribute, const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer) override;
	};
}
#endif