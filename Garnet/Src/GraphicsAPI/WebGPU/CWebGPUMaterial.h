#pragma once
#ifdef USE_WEBGPU
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../../Graphics/CMaterial.h"
#include <memory>

#include <webgpu.h>

namespace graphics{ class CMaterialCreateInfo; }

namespace api
{
	class CWebGPUAPI;
	class CWebGPUTexture;

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

		// Texture
		std::shared_ptr<CWebGPUTexture> m_EmptyTexture;
	private:
		// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
		bool CreateShaderStages(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateShaderBuffers(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		bool CreateBindGroup(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, 
			const std::vector<std::shared_ptr<graphics::CTexture>>& CubeMapList);

		// Helper Function ///////////////////////////////////////////////////////////////////////
		WGPUShaderModule CreateShaderModuleFromWGSL(const std::string& shaderCode);
		WGPUShaderModule CreateShaderModuleFromSPIRV(const std::vector<unsigned char>& shaderCode);
		bool             CreateWGUniformBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize);
		void			 InitDefalutBindGroupLayoutEntry(WGPUBindGroupLayoutEntry& bindingLayout);
	public:
		CWebGPUMaterial(api::CWebGPUAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		virtual ~CWebGPUMaterial();

		virtual bool Create(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, const std::vector<std::shared_ptr<graphics::CTexture>>& CubeMapList) override;
		virtual bool SetCommonUniform(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) override;
		virtual bool BuildDrawBuffer(int DynamicOffsetNum) override;

		virtual void SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum = -1) override;

		const WGPUShaderModule& GetVertexShaderModele() { return m_VertexShaderModele; }
		const WGPUShaderModule& GetFragmentShaderModele() { return m_FragmentShaderModele; }

		const WGPUBindGroupLayout& GetBindGroupLayout() { return m_BindGroupLayout; }
		const WGPUBindGroup& GetBindGroup() { return m_BindGroup; }

#ifdef USE_GPGPU
		virtual bool Dispatch(const glm::ivec3& Threads, const glm::ivec3& KernelSize) override;
#endif // USE_GPGPU
	};
}
#endif