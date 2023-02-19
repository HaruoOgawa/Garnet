#include "CRendererCreateInfo.h"

namespace renderer
{
	CRendererCreateInfo::CRendererCreateInfo():
		m_UseMainTexture(false)
	{
	}

	// Shader
	void CRendererCreateInfo::SetVertexShaderCode(const std::vector<unsigned char>& VertexShaderCode)
	{
		m_VertexShaderCode = VertexShaderCode;
	}

	const std::vector<unsigned char>& CRendererCreateInfo::GetVertexShaderCode() const
	{
		return m_VertexShaderCode;
	}

	void CRendererCreateInfo::SetFragmentShaderCode(const std::vector<unsigned char>& FragmentShaderCode)
	{
		m_FragmentShaderCode = FragmentShaderCode;
	}

	const std::vector<unsigned char>& CRendererCreateInfo::GetFragmentShaderCode() const
	{
		return m_FragmentShaderCode;
	}

	void CRendererCreateInfo::SetGeometryShaderCode(const std::vector<unsigned char>& GeometryShaderCode)
	{
		m_GeometryShaderCode = GeometryShaderCode;
	}

	const std::vector<unsigned char>& CRendererCreateInfo::GetGeometryShaderCode() const
	{
		return m_GeometryShaderCode;
	}

	void CRendererCreateInfo::HullShaderCode(const std::vector<unsigned char>& HullShaderCode)
	{
		m_HullShaderCode = HullShaderCode;
	}

	const std::vector<unsigned char>& CRendererCreateInfo::GetHullShaderCode() const
	{
		return m_HullShaderCode;
	}

	void CRendererCreateInfo::SetDomainShaderCode(const std::vector<unsigned char>& DomainShaderCode)
	{
		m_DomainShaderCode = DomainShaderCode;
	}

	const std::vector<unsigned char>& CRendererCreateInfo::GetDomainShaderCode() const
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