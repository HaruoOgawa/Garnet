#include "CMaterialCreateInfo.h"

namespace graphics
{
	CMaterialCreateInfo::CMaterialCreateInfo():
#ifdef __DAWN__
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
	}

	// Uniform
	std::shared_ptr<graphics::CUniformBuffer> CMaterialCreateInfo::CreateUniformBuffer(std::vector<int> BindIndexList)
	{
		auto Buffer = std::make_shared<graphics::CUniformBuffer>(BindIndexList);
		
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

	// Texture
}