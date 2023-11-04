#pragma once

#ifdef USE_OPENGL
#include <memory>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../Graphics/CMaterial.h"
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

#ifdef USE_TEXTURE_LOADER
		// Texture
		std::shared_ptr<COpenGLTexture> m_EmptyTexture;
#endif
		std::shared_ptr<graphics::CTextureSet> m_TextureSet;
	private:
		// Main Logics
		bool CreateShaderStages();
		bool CreateShaderBuffers();

		// Helper Functions
		static bool CompileShader(const std::vector<unsigned char>& shaderCode, GLenum shaderType, GLuint& shaderPrg);
		static std::string PreparePreprocessor();
	public:
		COpenGLMaterial(api::COpenGLAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		virtual ~COpenGLMaterial();

		virtual bool Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet) override;
		virtual bool BuildDrawBuffer(int DynamicOffsetNum) override;

		virtual void SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum = -1) override;

		void SetActive();

		// Uniform Buffer Object
		const std::vector<GLuint>& GetUBOList() const { return m_UBOList; }
	};
}
#endif // USE_OPENGL