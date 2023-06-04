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
		std::vector<uint32_t>	m_WGPUUniformBufferByteSizeList;

		// BindGroup
		WGPUBindGroupLayout m_BindGroupLayout;
		WGPUBindGroup m_BindGroup;
	private:
		// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
		bool CreateShaderStages(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateUniformBuffer(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateBindGroup(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList);

		// Helper Function ///////////////////////////////////////////////////////////////////////
		WGPUShaderModule CreateShaderModuleFromWGSL(const std::string& shaderCode);
		WGPUShaderModule CreateShaderModuleFromSPIRV(const std::vector<unsigned char>& shaderCode);
		bool             CreateWGUniformBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize);
		void			 InitDefalutBindGroupLayoutEntry(WGPUBindGroupLayoutEntry& bindingLayout);
	public:
		CWebGPUMaterial(api::CWebGPUAPI* pGraphicsAPI);
		virtual ~CWebGPUMaterial();

		virtual bool Create(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList) override;
		virtual bool Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) override;
		virtual bool BuildDrawBuffer(int DynamicOffsetNum) override;

		virtual void SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum = -1) override;

		const WGPUShaderModule& GetVertexShaderModele() { return m_VertexShaderModele; }
		const WGPUShaderModule& GetFragmentShaderModele() { return m_FragmentShaderModele; }

		const WGPUBindGroupLayout& GetBindGroupLayout() { return m_BindGroupLayout; }
		const WGPUBindGroup& GetBindGroup() { return m_BindGroup; }
	};
}
#endif