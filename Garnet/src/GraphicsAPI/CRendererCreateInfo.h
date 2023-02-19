#pragma once
#include <vector>

namespace renderer
{
	class CRendererCreateInfo
	{
		// VertexBuffer

		// Uniform

		// Rendering

		// Shader
		std::vector<unsigned char> m_VertexShaderCode;
		std::vector<unsigned char> m_FragmentShaderCode;
		std::vector<unsigned char> m_GeometryShaderCode;
		std::vector<unsigned char> m_HullShaderCode;
		std::vector<unsigned char> m_DomainShaderCode;

		// Texture
		bool m_UseMainTexture;
	public:
		CRendererCreateInfo();
		virtual ~CRendererCreateInfo() = default;
		
		// Shader
		void SetVertexShaderCode(const std::vector<unsigned char>& VertexShaderCode);
		const std::vector<unsigned char>& GetVertexShaderCode() const;

		void SetFragmentShaderCode(const std::vector<unsigned char>& FragmentShaderCode);
		const std::vector<unsigned char>& GetFragmentShaderCode() const;

		void SetGeometryShaderCode(const std::vector<unsigned char>& GeometryShaderCode);
		const std::vector<unsigned char>& GetGeometryShaderCode() const;

		void HullShaderCode(const std::vector<unsigned char>& HullShaderCode);
		const std::vector<unsigned char>& GetHullShaderCode() const;

		void SetDomainShaderCode(const std::vector<unsigned char>& DomainShaderCode);
		const std::vector<unsigned char>& GetDomainShaderCode() const;

		// Texture
		void SetUseMainTexture(bool UseMainTexture);
		bool IsUseMainTexture() const;
	};
}