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

	// Uniform
	std::shared_ptr<graphics::CUniformBuffer> CMaterialCreateInfo::CreateUniformBuffer(std::vector<int> BindIndexList)
	{
		auto Buffer = std::make_shared<graphics::CUniformBuffer>(BindIndexList);
		
		return Buffer;
	}
	std::shared_ptr<graphics::CTextureBuffer> CMaterialCreateInfo::CreateTextureBuffer(std::vector<int> BindIndexList)
	{
		auto Buffer = std::make_shared<graphics::CTextureBuffer>(BindIndexList);

		return Buffer;
	}

	// Shader
	EShaderType CMaterialCreateInfo::GetShaderType() const
	{
		return m_ShaderType;
	}

	void CMaterialCreateInfo::SetVertexShaderCode(const std::vector<char>& VertexShaderCode)
	{
		m_VertexShaderCode = VertexShaderCode;
	}

	const std::vector<char>& CMaterialCreateInfo::GetVertexShaderCode() const
	{
		return m_VertexShaderCode;
	}

	void CMaterialCreateInfo::SetFragmentShaderCode(const std::vector<char>& FragmentShaderCode)
	{
		m_FragmentShaderCode = FragmentShaderCode;
	}

	const std::vector<char>& CMaterialCreateInfo::GetFragmentShaderCode() const
	{
		return m_FragmentShaderCode;
	}

	void CMaterialCreateInfo::SetGeometryShaderCode(const std::vector<char>& GeometryShaderCode)
	{
		m_GeometryShaderCode = GeometryShaderCode;
	}

	const std::vector<char>& CMaterialCreateInfo::GetGeometryShaderCode() const
	{
		return m_GeometryShaderCode;
	}

	void CMaterialCreateInfo::HullShaderCode(const std::vector<char>& HullShaderCode)
	{
		m_HullShaderCode = HullShaderCode;
	}

	const std::vector<char>& CMaterialCreateInfo::GetHullShaderCode() const
	{
		return m_HullShaderCode;
	}

	void CMaterialCreateInfo::SetDomainShaderCode(const std::vector<char>& DomainShaderCode)
	{
		m_DomainShaderCode = DomainShaderCode;
	}

	const std::vector<char>& CMaterialCreateInfo::GetDomainShaderCode() const
	{
		return m_DomainShaderCode;
	}

	// Texture
}