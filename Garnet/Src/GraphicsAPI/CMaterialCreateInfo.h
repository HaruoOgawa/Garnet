#pragma once

#include <vector>
#include <string>
#include "../Interface/IBuffer.h"
#include "../Graphics/CShaderBuffer.h"

namespace graphics
{
	enum class EShaderType
	{
		WGSL,
		SPIRV,
		GLSL,
	};

	class CShaderBuffer;

	class CMaterialCreateInfo
	{
		// Shader
		const EShaderType m_ShaderType;

		std::vector<unsigned char> m_VertexShaderCode;
		std::vector<unsigned char> m_FragmentShaderCode;
		std::vector<unsigned char> m_GeometryShaderCode;
		std::vector<unsigned char> m_HullShaderCode;
		std::vector<unsigned char> m_DomainShaderCode;
		std::vector<unsigned char> m_ComputeShaderCode;
	public:
		CMaterialCreateInfo();
		virtual ~CMaterialCreateInfo();

		// Uniform
		static std::shared_ptr<graphics::CShaderBuffer> CreateUniformBuffer(const std::vector<SBindingLayout>& BindingLayoutList);
		static std::shared_ptr<graphics::CShaderBuffer> CreateShaderStorageBuffer(const std::vector<SBindingLayout>& BindingLayoutList, EBufferUpdateType BufferUpdateType = EBufferUpdateType::UPDATE_TYPE_GPU);

		// Shader
		EShaderType GetShaderType() const;

		void SetVertexShaderCode(const std::vector<unsigned char>& VertexShaderCode);
		const std::vector<unsigned char>& GetVertexShaderCode() const;

		void SetFragmentShaderCode(const std::vector<unsigned char>& FragmentShaderCode);
		const std::vector<unsigned char>& GetFragmentShaderCode() const;

		void SetGeometryShaderCode(const std::vector<unsigned char>& GeometryShaderCode);
		const std::vector<unsigned char>& GetGeometryShaderCode() const;

		void SetHullShaderCode(const std::vector<unsigned char>& HullShaderCode);
		const std::vector<unsigned char>& GetHullShaderCode() const;

		void SetDomainShaderCode(const std::vector<unsigned char>& DomainShaderCode);
		const std::vector<unsigned char>& GetDomainShaderCode() const;
		
		void SetComputeShaderCode(const std::vector<unsigned char>& ComputeShaderCode);
		const std::vector<unsigned char>& GetComputeShaderCode() const;
	};

}