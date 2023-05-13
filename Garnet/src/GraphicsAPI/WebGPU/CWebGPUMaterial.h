#pragma once
#ifdef __DAWN__
#include <webgpu.h>
#include <wgpu.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../../Graphics/CMaterial.h"
#include <memory>

namespace graphics{ class CMaterialCreateInfo; }

namespace api
{
	class CWebGPUAPI;

	class CWebGPUMaterial : public graphics::CMaterial
	{
		// API
		api::CWebGPUAPI* m_pGraphicsAPI;

		// Shader
		WGPUShaderModule m_VertexShaderModele;
		WGPUShaderModule m_FragmentShaderModele;

		// Uniform
		std::vector<WGPUBuffer> m_WGPUUniformBufferList;
		std::vector<size_t>	    m_WGPUUniformSizeList;

		// BindGroup
		WGPUBindGroupLayout m_BindGroupLayout;
		WGPUBindGroup m_BindGroup;
	private:
		// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
		bool CreateShaderStages(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateUniformBuffer(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateBindGroup(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);

		// Helper Function ///////////////////////////////////////////////////////////////////////
		WGPUShaderModule CreateShaderModuleFromWGSL(const std::string& shaderCode);
		WGPUShaderModule CreateShaderModuleFromSPIRV(const std::vector<char>& shaderCode);
		bool             CreateBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize);
		void			 InitDefalutBindGroupLayoutEntry(WGPUBindGroupLayoutEntry& bindingLayout);
	public:
		CWebGPUMaterial();
		virtual ~CWebGPUMaterial();

		virtual bool Create(api::IGraphicsAPI* pGraphicsAPI) override;
		virtual bool Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection) override;
		virtual bool BuildDrawBuffer() override;

		virtual void SetUniformValue(const std::string Name, const void* Value) override;

		const WGPUShaderModule& GetVertexShaderModele() { return m_VertexShaderModele; }
		const WGPUShaderModule& GetFragmentShaderModele() { return m_FragmentShaderModele; }

		const WGPUBindGroupLayout& GetBindGroupLayout() { return m_BindGroupLayout; }
		const WGPUBindGroup& GetBindGroup() { return m_BindGroup; }
	};
}
#endif