#ifdef USE_OPENGL
#include "COpenGLMaterial.h"
#include "COpenGLTexture.h"

namespace api
{
	COpenGLMaterial::COpenGLMaterial(api::COpenGLAPI* pGraphicsAPI):
		CMaterial(),
		m_pGraphicsAPI(pGraphicsAPI),

		m_ShaderPrg(-1)
	{
#ifdef USE_TEXTURE_LOADER
		m_EmptyTexture = std::make_shared<COpenGLTexture>(pGraphicsAPI, false);
		std::vector<unsigned char> emptyPixel = { 0, 0, 0, 0 };
		m_EmptyTexture->Create(emptyPixel, static_cast<int>(emptyPixel.size() * sizeof(unsigned char)));
#endif
	}

	COpenGLMaterial::~COpenGLMaterial()
	{
	}

	bool COpenGLMaterial::Create(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, const std::vector<std::shared_ptr<graphics::CTexture>>& CubeMapList)
	{
		if (!LoadShader()) return false;
		if (!InitializeUniformBuffer()) return false;

		return true;
	}

	bool COpenGLMaterial::SetCommonUniform(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		return true;
	}

	bool COpenGLMaterial::BuildDrawBuffer(int DynamicOffsetNum)
	{
		return true;
	}

	void COpenGLMaterial::SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum)
	{

	}

	// Main Logics //////////////////////////////////////////////////
	void COpenGLMaterial::SetActive()
	{
		glUseProgram(m_ShaderPrg);
	}

	bool COpenGLMaterial::LoadShader()
	{
		m_ShaderPrg = glCreateProgram();

		if (!CompileShader(m_CreateInfo->GetVertexShaderCode(), GL_VERTEX_SHADER, m_ShaderPrg)) return false;
		if (!CompileShader(m_CreateInfo->GetFragmentShaderCode(), GL_FRAGMENT_SHADER, m_ShaderPrg)) return false;
		if (!CompileShader(m_CreateInfo->GetGeometryShaderCode(), GL_GEOMETRY_SHADER, m_ShaderPrg)) return false;
		if (!CompileShader(m_CreateInfo->GetHullShaderCode(), GL_TESS_CONTROL_SHADER, m_ShaderPrg)) return false;
		if (!CompileShader(m_CreateInfo->GetDomainShaderCode(), GL_TESS_EVALUATION_SHADER, m_ShaderPrg)) return false;
		//if (!CompileShader(m_CreateInfo->GetComputeShaderCode(), GL_COMPUTE_SHADER, m_ShaderPrg)) return false;

		return true;
	}

	bool COpenGLMaterial::InitializeUniformBuffer()
	{
		SetActive();

		for (const auto& Buffer : m_UniformBufferList)
		{
			
		}
		
		for (const auto& TexLayout : m_TextureBindingLayoutList)
		{

		}

		return true;
	}

	// Helper Functions //////////////////////////////////////////////////
	bool COpenGLMaterial::CompileShader(const std::vector<unsigned char>& shaderCode, GLenum shaderType, GLuint& shaderPrg)
	{
		// 空のシェーダーはセーフ
		if (shaderCode.empty()) return true;

		// Shader Objectを生成
		GLuint shader = glCreateShader(shaderType);
		const char* content = reinterpret_cast<const char*>(&shaderCode[0]);
		glShaderSource(shader, 1, &(content), nullptr);
		glCompileShader(shader);

		// コンパイルエラーチェック
		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

		if (status != GL_TRUE)
		{
			char buffer[512];
			std::memset(buffer, 0, 512);
			glGetShaderInfoLog(shader, 512, nullptr, buffer);

			Console::Log("[Error] GLSL Compile Error - {Error Message: %s, ShaderCode: %s}\n", buffer, content);

			return false;
		}

		// コンパイルに成功したのでShader ProgramにShaderをアタッチする
		glAttachShader(shaderPrg, shader);

		return true;
	}
}
#endif // USE_OPENGL