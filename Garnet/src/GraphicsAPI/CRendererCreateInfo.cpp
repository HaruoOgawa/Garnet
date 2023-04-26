#include "CRendererCreateInfo.h"

namespace renderer
{
	CRendererCreateInfo::CRendererCreateInfo():
		m_UseMainTexture(false),
		m_ShaderType(EShaderType::SPIRV)
	{
	}

	// VertexBuffer
	void CRendererCreateInfo::SetVertices(const std::vector<std::vector<float>>& Vertices)
	{
		m_Vertices = Vertices;
	}

	const std::vector<std::vector<float>>& CRendererCreateInfo::GetVertices() const
	{
		return m_Vertices;
	}

	void CRendererCreateInfo::SetIndices(const std::vector<uint16_t>& Indices)
	{
		m_Indices = Indices;
	}

	const std::vector<uint16_t>& CRendererCreateInfo::GetIndices() const
	{
		return m_Indices;
	}

	void CRendererCreateInfo::SetAttributeDimensions(const std::vector<int>& AttributeDimensions)
	{
		m_AttributeDimensions = AttributeDimensions;
	}
	const std::vector<int>& CRendererCreateInfo::GetAttributeDimensions() const
	{
		return m_AttributeDimensions;
	}

	// Shader
	void CRendererCreateInfo::SetShaderType(EShaderType ShaderType)
	{
		m_ShaderType = ShaderType;
	}

	EShaderType CRendererCreateInfo::GetShaderType() const
	{
		return m_ShaderType;
	}

	void CRendererCreateInfo::SetVertexShaderCode(const std::vector<char>& VertexShaderCode)
	{
		m_VertexShaderCode = VertexShaderCode;
	}

	const std::vector<char>& CRendererCreateInfo::GetVertexShaderCode() const
	{
		return m_VertexShaderCode;
	}

	void CRendererCreateInfo::SetFragmentShaderCode(const std::vector<char>& FragmentShaderCode)
	{
		m_FragmentShaderCode = FragmentShaderCode;
	}

	const std::vector<char>& CRendererCreateInfo::GetFragmentShaderCode() const
	{
		return m_FragmentShaderCode;
	}

	void CRendererCreateInfo::SetGeometryShaderCode(const std::vector<char>& GeometryShaderCode)
	{
		m_GeometryShaderCode = GeometryShaderCode;
	}

	const std::vector<char>& CRendererCreateInfo::GetGeometryShaderCode() const
	{
		return m_GeometryShaderCode;
	}

	void CRendererCreateInfo::HullShaderCode(const std::vector<char>& HullShaderCode)
	{
		m_HullShaderCode = HullShaderCode;
	}

	const std::vector<char>& CRendererCreateInfo::GetHullShaderCode() const
	{
		return m_HullShaderCode;
	}

	void CRendererCreateInfo::SetDomainShaderCode(const std::vector<char>& DomainShaderCode)
	{
		m_DomainShaderCode = DomainShaderCode;
	}

	const std::vector<char>& CRendererCreateInfo::GetDomainShaderCode() const
	{
		return m_DomainShaderCode;
	}

	// Texture
	void CRendererCreateInfo::SetUseMainTexture(bool UseMainTexture)
	{
		m_UseMainTexture = UseMainTexture;
	}

	bool CRendererCreateInfo::IsUseMainTexture() const
	{
		return m_UseMainTexture;
	}
}