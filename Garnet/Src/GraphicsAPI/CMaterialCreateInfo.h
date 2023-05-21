#pragma once

#include <vector>
#include <string>
#include "../Interface/IBuffer.h"
#include "../Graphics/CUniformBuffer.h"

namespace graphics
{
	enum class EShaderType
	{
		WGSL,
		SPIRV,
		GLSL,
	};

	class CUniformBuffer;

	class CMaterialCreateInfo
	{
		// Shader
		const EShaderType m_ShaderType;

		std::vector<char> m_VertexShaderCode;
		std::vector<char> m_FragmentShaderCode;
		std::vector<char> m_GeometryShaderCode;
		std::vector<char> m_HullShaderCode;
		std::vector<char> m_DomainShaderCode;
	public:
		CMaterialCreateInfo();
		virtual ~CMaterialCreateInfo();

		// Uniform
		static std::shared_ptr<graphics::CUniformBuffer> CreateUniformBuffer(std::vector<int> BindIndexList);

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
	};

}