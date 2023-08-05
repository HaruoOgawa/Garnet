#ifdef USE_OPENGL
#include "COpenGLMaterial.h"
#include "COpenGLTexture.h"
#include "../CMaterialCreateInfo.h"
#include "../../Debug/Message/Console.h"
#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"
#include "../../Math/CTransform.h"
#include "../../Math/CMath.h"

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
		if (!CreateShaderStages()) return false;
		if (!CreateUniformBuffers()) return false;

		return true;
	}

	bool COpenGLMaterial::SetCommonUniform(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		glm::mat4 lightVPMat = DrawInfo->GetLightProjection()->GetPrejectionMatrix() * DrawInfo->GetLightCamera()->GetViewMatrix();

		// 共通のユニフォームバッファの更新
		SetUniformValue("view", &Camera->GetViewMatrix()[0][0]);
		SetUniformValue("proj", &Projection->GetPrejectionMatrix()[0][0]);
		SetUniformValue("lightVPMat", &lightVPMat[0][0]);
		SetUniformValue("lightDir", &DrawInfo->GetLightCamera()->GetViewDir()[0]);
		SetUniformValue("lightColor", &DrawInfo->GetLightColor()[0]);
		SetUniformValue("cameraPos", &Camera->GetPos()[0]);
		SetUniformValue("time", &SecondsTime);

		return true;
	}

	bool COpenGLMaterial::BuildDrawBuffer(int DynamicOffsetNum)
	{
		SetActive();

		int index = 0;
		for (const auto& Buffer : m_UniformBufferList)
		{
			for (const auto& Layout : Buffer->GetBindingLayoutList())
			{
				if (index >= m_UBOList.size()) continue;

				GLuint uboIndex = m_UBOList[index];

				// UBOを生成
				/*GLuint uboIndex;
				glGenBuffers(1, &uboIndex);*/

				// Uniformのbinding indexを割り当てる
				GLuint blockIndex = glGetUniformBlockIndex(m_ShaderPrg, Layout.second.BindingName.c_str());
				glUniformBlockBinding(m_ShaderPrg, blockIndex, Layout.second.BindingIndex); // ShaderPrgとBinding Blockを紐づける
				//glBindBufferBase(GL_UNIFORM_BUFFER, Layout.second.BindingIndex, uboIndex); // UBOとBinding Blockを紐づける
				glBindBufferRange(GL_UNIFORM_BUFFER, Layout.second.BindingIndex, uboIndex, 0, Layout.second.ByteSize); // UBOとBinding Blockを紐づける

				// データの受け渡し
				glBindBuffer(GL_UNIFORM_BUFFER, uboIndex);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, Layout.second.ByteSize, &Buffer->GetData()[Layout.second.ByteOffset]); // Bufferのデータを更新
				//glBufferData(GL_UNIFORM_BUFFER, Layout.second.ByteSize, &Buffer->GetData()[Layout.second.ByteOffset], GL_STATIC_DRAW); // Bufferのデータを初期化・メモリ確保
				glBindBuffer(GL_UNIFORM_BUFFER, 0);

				index++;
			}
		}

		return true;
	}

	void COpenGLMaterial::SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum)
	{
		for (int i = 0; i < m_UniformBufferList.size(); i++)
		{
			auto& UniformBuffer = m_UniformBufferList[i];
			const auto& UniformDesc = UniformBuffer->GetDescriptor();

			const auto& DataList = UniformDesc->GetDataList();
			const auto& UniformData = DataList.find(Name);
			if (UniformData != DataList.end())
			{
				const int ByteOffset = UniformData->second.ByteOffset;
				const int ByteSize = UniformData->second.ByteSize;

				UniformBuffer->SetValue(Value, ByteOffset, ByteSize);
			}
		}
	}

	// Main Logics //////////////////////////////////////////////////
	void COpenGLMaterial::SetActive()
	{
		glUseProgram(m_ShaderPrg);
	}

	bool COpenGLMaterial::CreateShaderStages()
	{
		// ShaderProgramを作成
		m_ShaderPrg = glCreateProgram();

		// Shaderをコンパイル
		if (!CompileShader(m_CreateInfo->GetVertexShaderCode(), GL_VERTEX_SHADER, m_ShaderPrg)) return false;
		if (!CompileShader(m_CreateInfo->GetFragmentShaderCode(), GL_FRAGMENT_SHADER, m_ShaderPrg)) return false;
		if (!CompileShader(m_CreateInfo->GetGeometryShaderCode(), GL_GEOMETRY_SHADER, m_ShaderPrg)) return false;
		if (!CompileShader(m_CreateInfo->GetHullShaderCode(), GL_TESS_CONTROL_SHADER, m_ShaderPrg)) return false;
		if (!CompileShader(m_CreateInfo->GetDomainShaderCode(), GL_TESS_EVALUATION_SHADER, m_ShaderPrg)) return false;
		//if (!CompileShader(m_CreateInfo->GetComputeShaderCode(), GL_COMPUTE_SHADER, m_ShaderPrg)) return false;

		// ShaderをProgramにリンクする
		glLinkProgram(m_ShaderPrg);

		return true;
	}

	bool COpenGLMaterial::CreateUniformBuffers()
	{
		SetActive();

		for (const auto& Buffer : m_UniformBufferList)
		{
			for (const auto& Layout : Buffer->GetBindingLayoutList())
			{
				// UBOを生成
				GLuint uboIndex;
				glGenBuffers(1, &uboIndex);

				// Uniformのbinding indexを割り当てる
				GLuint blockIndex = glGetUniformBlockIndex(m_ShaderPrg, Layout.second.BindingName.c_str());
				glUniformBlockBinding(m_ShaderPrg, blockIndex, Layout.second.BindingIndex); // ShaderPrgとBinding Blockを紐づける
				//glBindBufferBase(GL_UNIFORM_BUFFER, Layout.second.BindingIndex, uboIndex); // UBOとBinding Blockを紐づける
				glBindBufferRange(GL_UNIFORM_BUFFER, Layout.second.BindingIndex, uboIndex, 0, Layout.second.ByteSize); // UBOとBinding Blockを紐づける

				// データの受け渡し
				glBindBuffer(GL_UNIFORM_BUFFER, uboIndex);
				glBufferData(GL_UNIFORM_BUFFER, Layout.second.ByteSize, &Buffer->GetData()[Layout.second.ByteOffset], GL_STATIC_DRAW); // Bufferのデータを初期化・メモリ確保
				//glBufferSubData(GL_UNIFORM_BUFFER, 0, Layout.second.ByteSize, &Buffer->GetData()[Layout.second.ByteOffset]); // Bufferのデータを更新
				glBindBuffer(GL_UNIFORM_BUFFER, 0);

				//
				m_UBOList.push_back(uboIndex);
			}
		}
		
		// テクスチャの扱いは後で考える
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

		std::string code_str = std::string(shaderCode.begin(), shaderCode.end());
		const char* content = code_str.c_str();

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