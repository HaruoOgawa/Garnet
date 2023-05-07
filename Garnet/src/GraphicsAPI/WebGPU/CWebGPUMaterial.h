#pragma once
#ifdef __DAWN__
#include <webgpu.h>
#include <wgpu.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../../Graphics/CMaterial.h"

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
		std::vector<WGPUBuffer> m_UniformBufferList;
		std::vector<size_t>	    m_UniformSizeList;

		// BindGroup
		WGPUBindGroupLayout m_BindGroupLayout;
		WGPUBindGroup m_BindGroup;
	private:
		// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
		bool CreateShaderStages(const graphics::CMaterialCreateInfo& createInfo);
		bool CreateUniformBuffer(const graphics::CMaterialCreateInfo& createInfo);
		bool CreateBindGroup(const graphics::CMaterialCreateInfo& createInfo);

		// Helper Function ///////////////////////////////////////////////////////////////////////
		WGPUShaderModule CreateShaderModuleFromWGSL(const std::string& shaderCode);
		WGPUShaderModule CreateShaderModuleFromSPIRV(const std::vector<char>& shaderCode);
		bool             CreateBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize);
		void			 InitDefalutBindGroupLayoutEntry(WGPUBindGroupLayoutEntry& bindingLayout);
	public:
		CWebGPUMaterial();
		virtual ~CWebGPUMaterial();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const graphics::CMaterialCreateInfo& createInfo) override;
		bool Update(float SecondsTime) override;

		const WGPUShaderModule& GetVertexShaderModele() { return m_VertexShaderModele; }
		const WGPUShaderModule& GetFragmentShaderModele() { return m_FragmentShaderModele; }

		const WGPUBindGroupLayout& GetBindGroupLayout() { return m_BindGroupLayout; }
		const WGPUBindGroup& GetBindGroup() { return m_BindGroup; }
	};
}
#endif