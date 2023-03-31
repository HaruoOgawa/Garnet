#pragma once
#include <vector>
#include <string>

namespace renderer
{
	class CRendererCreateInfo
	{
		// VertexBuffer
		std::vector<std::vector<float>> m_Vertices;
		std::vector<uint16_t> m_Indices;
		std::vector<int> m_AttributeDimensions;

		// Uniform

		// Rendering

		// Shader
		std::string m_VertexShaderCode;
		std::string m_FragmentShaderCode;
		std::string m_GeometryShaderCode;
		std::string m_HullShaderCode;
		std::string m_DomainShaderCode;

		// Texture
		bool m_UseMainTexture;
	public:
		CRendererCreateInfo();
		virtual ~CRendererCreateInfo() = default;
		
		// VertexBuffer
		void SetVertices(const std::vector<std::vector<float>>& Vertices);
		const std::vector<std::vector<float>>& GetVertices() const;

		void SetIndices(const std::vector<uint16_t>& Indices);
		const std::vector<uint16_t>& GetIndices() const;

		void SetAttributeDimensions(const std::vector<int>& AttributeDimensions);
		const std::vector<int>& GetAttributeDimensions() const;

		// Shader
		void SetVertexShaderCode(const std::string& VertexShaderCode);
		const std::string& GetVertexShaderCode() const;

		void SetFragmentShaderCode(const std::string& FragmentShaderCode);
		const std::string& GetFragmentShaderCode() const;

		void SetGeometryShaderCode(const std::string& GeometryShaderCode);
		const std::string& GetGeometryShaderCode() const;

		void HullShaderCode(const std::string& HullShaderCode);
		const std::string& GetHullShaderCode() const;

		void SetDomainShaderCode(const std::string& DomainShaderCode);
		const std::string& GetDomainShaderCode() const;

		// Texture
		void SetUseMainTexture(bool UseMainTexture);
		bool IsUseMainTexture() const;
	};
}