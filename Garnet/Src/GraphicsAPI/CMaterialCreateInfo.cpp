#include "CMaterialCreateInfo.h"

namespace graphics
{
	CMaterialCreateInfo::CMaterialCreateInfo():
#ifdef USE_WEBGPU
		m_ShaderType(EShaderType::WGSL)
#else
		m_ShaderType(EShaderType::SPIRV)
#endif
	{
	}

	CMaterialCreateInfo::~CMaterialCreateInfo()
	{
		m_VertexShaderCode.clear();
		m_FragmentShaderCode.clear();
		m_GeometryShaderCode.clear();
		m_HullShaderCode.clear();
		m_DomainShaderCode.clear();
		m_ComputeShaderCode.clear();
	}

	// Uniform
	std::shared_ptr<graphics::CShaderBuffer> CMaterialCreateInfo::CreateUniformBuffer(const std::vector<SBindingLayout>& BindingLayoutList)
	{
		// UniformBufferÇ÷ÇÃèëÇ´çûÇ›ÇÕCPUÇÃÇ›
		auto Buffer = std::make_shared<graphics::CShaderBuffer>(EBufferType::UNIFORM, BindingLayoutList, EBufferUpdateType::UPDATE_TYPE_CPU);
		
		return Buffer;
	}
	
	std::shared_ptr<graphics::CShaderBuffer> CMaterialCreateInfo::CreateShaderStorageBuffer(const std::vector<SBindingLayout>& BindingLayoutList, EBufferUpdateType BufferUpdateType)
	{
		auto Buffer = std::make_shared<graphics::CShaderBuffer>(EBufferType::SHADERSTORAGE, BindingLayoutList, BufferUpdateType);
		
		return Buffer;
	}

	// Shader
	EShaderType CMaterialCreateInfo::GetShaderType() const
	{
		return m_ShaderType;
	}

	void CMaterialCreateInfo::SetVertexShaderCode(const std::vector<unsigned char>& VertexShaderCode)
	{
		m_VertexShaderCode = VertexShaderCode;
	}

	const std::vector<unsigned char>& CMaterialCreateInfo::GetVertexShaderCode() const
	{
		return m_VertexShaderCode;
	}

	void CMaterialCreateInfo::SetFragmentShaderCode(const std::vector<unsigned char>& FragmentShaderCode)
	{
		m_FragmentShaderCode = FragmentShaderCode;
	}

	const std::vector<unsigned char>& CMaterialCreateInfo::GetFragmentShaderCode() const
	{
		return m_FragmentShaderCode;
	}

	void CMaterialCreateInfo::SetGeometryShaderCode(const std::vector<unsigned char>& GeometryShaderCode)
	{
		m_GeometryShaderCode = GeometryShaderCode;
	}

	const std::vector<unsigned char>& CMaterialCreateInfo::GetGeometryShaderCode() const
	{
		return m_GeometryShaderCode;
	}

	void CMaterialCreateInfo::HullShaderCode(const std::vector<unsigned char>& HullShaderCode)
	{
		m_HullShaderCode = HullShaderCode;
	}

	const std::vector<unsigned char>& CMaterialCreateInfo::GetHullShaderCode() const
	{
		return m_HullShaderCode;
	}

	void CMaterialCreateInfo::SetDomainShaderCode(const std::vector<unsigned char>& DomainShaderCode)
	{
		m_DomainShaderCode = DomainShaderCode;
	}

	const std::vector<unsigned char>& CMaterialCreateInfo::GetDomainShaderCode() const
	{
		return m_DomainShaderCode;
	}

	void CMaterialCreateInfo::SetComputeShaderCode(const std::vector<unsigned char>& ComputeShaderCode)
	{
		m_ComputeShaderCode = ComputeShaderCode;
	}
	const std::vector<unsigned char>& CMaterialCreateInfo::GetComputeShaderCode() const
	{
		return m_ComputeShaderCode;
	}

	// Texture
}