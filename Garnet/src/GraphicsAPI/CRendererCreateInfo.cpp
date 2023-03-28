#include "CRendererCreateInfo.h"

namespace renderer
{
	CRendererCreateInfo::CRendererCreateInfo():
		m_UseMainTexture(false)
	{
	}

	// VertexBuffer
	void CRendererCreateInfo::SetVertices(const std::vector<SVertex>& Vertices)
	{
		m_Vertices = Vertices;
	}

	const std::vector<SVertex>& CRendererCreateInfo::GetVertices() const
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

	// Shader
	void CRendererCreateInfo::SetVertexShaderCode(const std::string& VertexShaderCode)
	{
		m_VertexShaderCode = VertexShaderCode;
	}

	const std::string& CRendererCreateInfo::GetVertexShaderCode() const
	{
		return m_VertexShaderCode;
	}

	void CRendererCreateInfo::SetFragmentShaderCode(const std::string& FragmentShaderCode)
	{
		m_FragmentShaderCode = FragmentShaderCode;
	}

	const std::string& CRendererCreateInfo::GetFragmentShaderCode() const
	{
		return m_FragmentShaderCode;
	}

	void CRendererCreateInfo::SetGeometryShaderCode(const std::string& GeometryShaderCode)
	{
		m_GeometryShaderCode = GeometryShaderCode;
	}

	const std::string& CRendererCreateInfo::GetGeometryShaderCode() const
	{
		return m_GeometryShaderCode;
	}

	void CRendererCreateInfo::HullShaderCode(const std::string& HullShaderCode)
	{
		m_HullShaderCode = HullShaderCode;
	}

	const std::string& CRendererCreateInfo::GetHullShaderCode() const
	{
		return m_HullShaderCode;
	}

	void CRendererCreateInfo::SetDomainShaderCode(const std::string& DomainShaderCode)
	{
		m_DomainShaderCode = DomainShaderCode;
	}

	const std::string& CRendererCreateInfo::GetDomainShaderCode() const
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