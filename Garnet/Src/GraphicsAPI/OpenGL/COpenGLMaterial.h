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
		std::vector<std::shared_ptr<graphics::CTexture>> m_TextureList;
		std::vector<std::shared_ptr<graphics::CTexture>> m_CubeMapList;
	private:
		// Main Logics
		bool CreateShaderStages();
		bool CreateUniformBuffers(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, const std::vector<std::shared_ptr<graphics::CTexture>>& CubeMapList);

		// Helper Functions
		static bool CompileShader(const std::vector<unsigned char>& shaderCode, GLenum shaderType, GLuint& shaderPrg);
		static std::string PreparePreprocessor();
	public:
		COpenGLMaterial(api::COpenGLAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		virtual ~COpenGLMaterial();

		virtual bool Create(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, const std::vector<std::shared_ptr<graphics::CTexture>>& CubeMapList) override;
		virtual bool SetCommonUniform(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) override;
		virtual bool BuildDrawBuffer(int DynamicOffsetNum) override;

		virtual void SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum = -1) override;

		void SetActive();

#ifdef USE_GPGPU
		virtual bool Dispatch(const glm::ivec3& Threads, const glm::ivec3& KernelSize) override;
#endif // USE_GPGPU
	};
}
#endif // USE_OPENGL