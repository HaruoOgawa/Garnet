#pragma once

#include <vector>
#include <string>
#include "../Interface/IBuffer.h"
#include "../Graphics/CUniformBuffer.h"
#include "../Graphics/CTextureBuffer.h"

namespace graphics
{
	enum class EShaderType
	{
		WGSL,
		SPIRV,
		GLSL,
	};

	class CUniformBuffer;
	class CTextureBuffer;

	class CMaterialCreateInfo
	{
		// Uniform
		std::vector<std::shared_ptr<graphics::IBuffer>> m_BufferList;

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
		CMaterialCreateInfo();
		virtual ~CMaterialCreateInfo() = default;

		// Uniform
		std::shared_ptr<graphics::IBuffer> CreateUniformBuffer(std::vector<int> BindIndexList);
		std::shared_ptr<graphics::IBuffer> CreateTextureBuffer(std::vector<int> BindIndexList);
		void AddBuffer(const std::shared_ptr<graphics::IBuffer>& Buffer);
		const std::vector<std::shared_ptr<graphics::IBuffer>>& GetBufferList() const;

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