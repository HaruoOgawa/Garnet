#pragma once

#ifdef USE_OPENGL
#include <memory>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../Graphics/CMaterial.h"
#include "../../Graphics/EShaderStage.h"
#include "COpenGLAPI.h"

namespace graphics { class CMaterialCreateInfo; }

namespace api
{
	class COpenGLTexture;

	class COpenGLMaterial : public graphics::CMaterial
	{
		// API
		api::COpenGLAPI* m_pGraphicsAPI;

		// Shader Program
		GLuint m_ShaderPrg;

		// Uniform Buffer Object
		std::vector<GLuint> m_UBOList;
	private:
		void Release();

		// Main Logics
		bool CreateShaderStages();
		bool CreateShaderBuffers();

		static std::string PreparePreprocessor();

		static bool CompileAndAttachShader(const std::vector<unsigned char>& shaderCode, GLuint& shaderPrg, graphics::EShaderStage ShaderStage, GLuint& shader);
		static void AttachShader(GLuint& shaderPrg, GLuint& shader);
	public:
		static bool CompileShader(const std::vector<unsigned char>& shaderCode, graphics::EShaderStage ShaderStage, GLuint& shader, std::string& ErrorMsg);

		COpenGLMaterial(api::COpenGLAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, graphics::ECullMode CullMode);
		virtual ~COpenGLMaterial();

		virtual bool Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet) override;

		virtual bool ReCreate(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::vector<std::shared_ptr<graphics::CShaderBuffer>>& ShaderBufferList, const std::vector<graphics::STextureBindingLayout>& TextureBindingLayoutList) override;

		virtual bool BuildDrawBuffer() override;

		virtual void SetUniformValue(const std::string Name, const void* Data, int ByteSize) override;

		void SetActive();

		// Uniform Buffer Object
		const std::vector<GLuint>& GetUBOList() const { return m_UBOList; }
	};
}
#endif // USE_OPENGL