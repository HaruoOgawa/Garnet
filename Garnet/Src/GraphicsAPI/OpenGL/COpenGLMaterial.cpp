#ifdef USE_OPENGL
#include "COpenGLMaterial.h"
#include "COpenGLTexture.h"
#include "../CMaterialCreateInfo.h"
#include "../../Debug/Message/Console.h"
#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"
#include "../../Math/CTransform.h"
#include "COpenGLTexture.h"

namespace api
{
	COpenGLMaterial::COpenGLMaterial(api::COpenGLAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo):
		CMaterial(createInfo),
		m_pGraphicsAPI(pGraphicsAPI),

		m_ShaderPrg(-1),
		m_TextureSet(nullptr)
	{
#ifdef USE_TEXTURE_LOADER
		m_EmptyTexture = std::make_shared<COpenGLTexture>(pGraphicsAPI, false);
		std::vector<unsigned char> emptyPixel = { 0, 0, 0, 0 };
		m_EmptyTexture->Create(emptyPixel, static_cast<int>(emptyPixel.size() * sizeof(unsigned char)));
#endif
	}

	COpenGLMaterial::~COpenGLMaterial()
	{
		glDeleteProgram(m_ShaderPrg);
	}

	bool COpenGLMaterial::Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		if (!CreateShaderStages()) return false;
		if (!CreateShaderBuffers()) return false;

		m_TextureSet = TextureSet;

		return true;
	}

	bool COpenGLMaterial::SetCommonUniform(const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		glm::mat4 lightVPMat = DrawInfo->GetLightProjection()->GetPrejectionMatrix() * DrawInfo->GetLightCamera()->GetViewMatrix();

		// 共通のユニフォームバッファの更新
		SetUniformValue("view", &Camera->GetViewMatrix()[0][0]);
		SetUniformValue("proj", &Projection->GetPrejectionMatrix()[0][0]);
		SetUniformValue("lightVPMat", &lightVPMat[0][0]);
		SetUniformValue("lightDir", &DrawInfo->GetLightCamera()->GetViewDir()[0]);
		SetUniformValue("lightColor", &DrawInfo->GetLightColor()[0]);
		SetUniformValue("cameraPos", &Camera->GetPos()[0]);
		SetUniformValue("time", &glm::vec1(DrawInfo->GetSecondsTime())[0]);
		SetUniformValue("deltaTime", &glm::vec1(DrawInfo->GetDeltaSecondsTime())[0]);

		return true;
	}

	bool COpenGLMaterial::BuildDrawBuffer(int DynamicOffsetNum)
	{
		SetActive();

		int index = 0;
		for (const auto& Buffer : m_ShaderBufferList)
		{
			// SharedBufferは処理しない
			if (Buffer->GetSharedBufferParam().IsShared) continue;

			if (Buffer->GetBufferType() == graphics::EBufferType::SHADERSTORAGE) continue;

			for (const auto& Layout : Buffer->GetBindingLayoutList())
			{
				if (index >= m_UBOList.size()) continue;

				GLuint uboIndex = m_UBOList[index];

				// Uniformのbinding indexを割り当てる
				GLuint blockIndex = glGetUniformBlockIndex(m_ShaderPrg, Layout.second.BindingName.c_str());
				glUniformBlockBinding(m_ShaderPrg, blockIndex, Layout.second.BindingIndex); // ShaderPrgとBinding Blockを紐づける
				glBindBufferRange(GL_UNIFORM_BUFFER, Layout.second.BindingIndex, uboIndex, 0, Layout.second.ByteSize); // UBOとBinding Blockを紐づける

				// データの受け渡し
				glBindBuffer(GL_UNIFORM_BUFFER, uboIndex);
				//glBufferData(GL_UNIFORM_BUFFER, Layout.second.ByteSize, &Buffer->GetData()[Layout.second.ByteOffset], GL_STATIC_DRAW); // Bufferのデータを初期化・メモリ確保
				glBufferSubData(GL_UNIFORM_BUFFER, 0, Layout.second.ByteSize, &Buffer->GetData()[Layout.second.ByteOffset]); // Bufferのデータを更新
				glBindBuffer(GL_UNIFORM_BUFFER, 0);

				index++;
			}
		}

		// テクスチャをShaderにバインドする
		std::vector<std::shared_ptr<graphics::CTexture>> TextureList(0);
		if (m_TextureSet) TextureList = m_TextureSet->Get2DTextureList();

		std::vector<std::shared_ptr<graphics::CTexture>> CubeMapList(0);
		if (m_TextureSet) CubeMapList = m_TextureSet->GetCubeMapList();

		std::vector<std::shared_ptr<graphics::CTexture>> FrameTextureList(0);
		if (m_TextureSet) FrameTextureList = m_TextureSet->GetFrameTextureList();

		int TexOrderIndex = 0;
		for (const auto& TexLayout : m_TextureBindingLayoutList)
		{
			COpenGLTexture* Texture = nullptr;
			int TextureIndex = TexLayout.TextureIndex;

#ifdef USE_TEXTURE_LOADER
			if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_2D)
			{
				Texture = (TextureIndex >= 0 && TextureIndex < TextureList.size()) ? static_cast<api::COpenGLTexture*>(TextureList[TextureIndex].get()) : m_EmptyTexture.get();
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_CUBE)
			{
				Texture = (TextureIndex >= 0 && TextureIndex < CubeMapList.size()) ? static_cast<api::COpenGLTexture*>(CubeMapList[TextureIndex].get()) : m_EmptyTexture.get();
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_FRAME)
			{
				Texture = (TextureIndex >= 0 && TextureIndex < FrameTextureList.size()) ? static_cast<api::COpenGLTexture*>(FrameTextureList[TextureIndex].get()) : m_EmptyTexture.get();
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL)
			{
				// あとで実装
			}
#else
			if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_2D)
			{
				Texture = (TextureIndex >= 0 && TextureIndex < TextureList.size()) ? static_cast<api::COpenGLTexture*>(TextureList[TextureIndex].get()) : nullptr;
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_CUBE)
			{
				Texture = (TextureIndex >= 0 && TextureIndex < CubeMapList.size()) ? static_cast<api::COpenGLTexture*>(CubeMapList[TextureIndex].get()) : nullptr;
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_FRAME)
			{
				Texture = (TextureIndex >= 0 && TextureIndex < FrameTextureList.size()) ? static_cast<api::COpenGLTexture*>(FrameTextureList[TextureIndex].get()) : nullptr;
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL)
			{
				// あとで実装
			}
#endif
			
			if (!Texture)
			{
				Console::Log("[ERROR] Texture is nullpte\n");
				return false;
			}

			Texture->SetActive(GL_TEXTURE0 + TexOrderIndex);

			GLuint location = glGetUniformLocation(m_ShaderPrg, TexLayout.TextureName.c_str());
			glUniform1i(location, TexOrderIndex);

			Texture->SetEactive(GL_TEXTURE0 + TexOrderIndex);

			TexOrderIndex++;
		}

		return true;
	}

	void COpenGLMaterial::SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum)
	{
		for (int i = 0; i < m_ShaderBufferList.size(); i++)
		{
			// SharedBufferは処理しない
			if (m_ShaderBufferList[i]->GetSharedBufferParam().IsShared) continue;

			auto& UniformBuffer = m_ShaderBufferList[i];
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
		if (!CompileShader(m_CreateInfo->GetComputeShaderCode(), GL_COMPUTE_SHADER, m_ShaderPrg)) return false;

		// ShaderをProgramにリンクする
		glLinkProgram(m_ShaderPrg);

		// リンクステータスをチェックする
		// このエラーハンドリングの仕方は覚えておくと便利かも
		GLint status;
		glGetProgramiv(m_ShaderPrg, GL_LINK_STATUS, &status);

		if (status != GL_TRUE)
		{
			char buffer[512];
			std::memset(buffer, 0, 512);
			glGetProgramInfoLog(m_ShaderPrg, 512, nullptr, buffer);

			Console::Log("[Error] Program Link Error - {Error Message: %s}\n", buffer);

			return false;
		}

		return true;
	}

	bool COpenGLMaterial::CreateShaderBuffers()
	{
		SetActive();

		for (const auto& Buffer : m_ShaderBufferList)
		{
			// 共有バッファ
			const auto& SharedBufferParam = Buffer->GetSharedBufferParam();
			
			//
			for (const auto& Layout : Buffer->GetBindingLayoutList())
			{
				GLenum target;
				GLenum usage;

				switch (Buffer->GetBufferType())
				{
					case graphics::EBufferType::UNIFORM:
						{
							target = GL_UNIFORM_BUFFER;
							usage = GL_STATIC_DRAW;
						}
						break;

					case graphics::EBufferType::SHADERSTORAGE:
						{
							target = GL_SHADER_STORAGE_BUFFER;
							usage = GL_DYNAMIC_DRAW;
						}
						break;

					default:
						{
							target = GL_UNIFORM_BUFFER;
							usage = GL_STATIC_DRAW;
						}
						break;
				}

				if (SharedBufferParam.IsShared) // バッファを他のマテリアルと共有する
				{
					COpenGLMaterial* pSharedOpenGLMat = static_cast<COpenGLMaterial*>(SharedBufferParam.SharedBufferMaterial.get());
					GLuint sharedUboIndex = pSharedOpenGLMat->GetUBOList()[SharedBufferParam.BufferIndex];

					// Uniformのbinding indexを割り当てる
					GLuint blockIndex = glGetUniformBlockIndex(m_ShaderPrg, Layout.second.BindingName.c_str());
					glUniformBlockBinding(m_ShaderPrg, blockIndex, Layout.second.BindingIndex); // ShaderPrgとBinding Blockを紐づける
					glBindBufferRange(target, Layout.second.BindingIndex, sharedUboIndex, 0, Layout.second.ByteSize); // UBOとBinding Blockを紐づける

					m_UBOList.push_back(sharedUboIndex);
				}
				else // 通常のバッファ使用
				{
					// UBOを生成
					GLuint uboIndex;
					glGenBuffers(1, &uboIndex);

					// Uniformのbinding indexを割り当てる
					GLuint blockIndex = glGetUniformBlockIndex(m_ShaderPrg, Layout.second.BindingName.c_str());
					glUniformBlockBinding(m_ShaderPrg, blockIndex, Layout.second.BindingIndex); // ShaderPrgとBinding Blockを紐づける
					glBindBufferRange(target, Layout.second.BindingIndex, uboIndex, 0, Layout.second.ByteSize); // UBOとBinding Blockを紐づける

					// データの受け渡し
					glBindBuffer(target, uboIndex);
					glBufferData(target, Layout.second.ByteSize, &Buffer->GetData()[Layout.second.ByteOffset], usage); // Bufferのデータを初期化・メモリ確保
					glBindBuffer(target, 0);

					m_UBOList.push_back(uboIndex);
				}
			}
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
		
		// プリプロセッサを差し込む位置を検索する(Versionの後)
		int firstNextLine = static_cast<int>(code_str.find_first_of('\n'));
		//Console::Log("firstNextLine: %d\n", firstNextLine);

		code_str = code_str.substr(0, firstNextLine) + "\n" + PreparePreprocessor() + code_str.substr(firstNextLine + 1, code_str.size() - firstNextLine - 1);
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

			Console::Log("[Error] GLSL Compile Error - {Error Message: %s}\n", buffer);
			Console::Log("[Error] ShaderCode: {%s}\n", content);

			return false;
		}

		// コンパイルに成功したのでShader ProgramにShaderをアタッチする
		glAttachShader(shaderPrg, shader);

		// アタッチしたので削除する
		glDeleteShader(shader);

		return true;
	}

	std::string COpenGLMaterial::PreparePreprocessor()
	{
		std::string preprocessor = "";

		preprocessor += "#define USE_OPENGL\n";

		return preprocessor;
	}
}
#endif // USE_OPENGL