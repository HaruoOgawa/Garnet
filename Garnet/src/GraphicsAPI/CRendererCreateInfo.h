#pragma once
#include <vector>
#include <string>

namespace renderer
{
	enum class EShaderType
	{
		WGSL,
		SPIRV,
		GLSL,
	};

	class CRendererCreateInfo
	{
		// VertexBuffer
		std::vector<std::vector<float>> m_Vertices;
		std::vector<uint16_t> m_Indices;
		std::vector<int> m_AttributeDimensions;

		// Uniform

		// Rendering

		// Shader
		const EShaderType m_ShaderType;

		std::vector<char> m_VertexShaderCode;
		std::vector<char> m_FragmentShaderCode;
		std::vector<char> m_GeometryShaderCode;
		std::vector<char> m_HullShaderCode;
		std::vector<char> m_DomainShaderCode;

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
		EShaderType GetShaderType() const;

		void SetVertexShaderCode(const std::vector<char>& VertexShaderCode);
		const std::vector<char>& GetVertexShaderCode() const;

		void SetFragmentShaderCode(const std::vector<char>& FragmentShaderCode);
		const std::vector<char>& GetFragmentShaderCode() const;

		void SetGeometryShaderCode(const std::vector<char>& GeometryShaderCode);
		const std::vector<char>& GetGeometryShaderCode() const;

		void HullShaderCode(const std::vector<char>& HullShaderCode);
		const std::vector<char>& GetHullShaderCode() const;

		void SetDomainShaderCode(const std::vector<char>& DomainShaderCode);
		const std::vector<char>& GetDomainShaderCode() const;

		// Texture
		void SetUseMainTexture(bool UseMainTexture);
		bool IsUseMainTexture() const;
	};
}