#pragma once
#ifdef USE_WEBGPU
#include "../../Interface/IRenderer.h"
#include <vector>
#include <string>
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

		//
		const std::string m_PassName;
		int m_InstanceCount;

		// Pipeline
		WGPURenderPipeline m_GraphicsPipeline;
	private:
		// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
		bool CreateGraphicsPipeline(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, api::CWebGPUMaterial* pWebGPUMat);
		
		// Helper Function ///////////////////////////////////////////////////////////////////////
		WGPUVertexFormat GetVertexFormat(int Dimention, graphics::EDataType DataType);
		void			 SetDefaultDepthStencil(WGPUDepthStencilState& depthStencilState);
	public:
		CWebGPURenderer(api::CWebGPUAPI* pGraphicsAPI, const std::string& PassName);
		virtual ~CWebGPURenderer();

		bool Create(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material) override;
		bool Draw(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material, int DynamicOffsetNum) override;
	};
}
#endif