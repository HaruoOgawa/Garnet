#ifdef USE_OPENGL
#include "COpenGLMaterial.h"
#include "COpenGLTexture.h"
#include "../CMaterialCreateInfo.h"
#include "../../Message/Console.h"
#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"
#include "../../Math/CTransform.h"
#include "COpenGLTexture.h"

namespace api
{
	COpenGLMaterial::COpenGLMaterial(api::COpenGLAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, int RefCount, graphics::ECullMode CullMode):
		CMaterial(pGraphicsAPI, createInfo, RefCount, CullMode),
		m_pGraphicsAPI(pGraphicsAPI),

		m_ShaderPrg(-1)
	{
	}

	COpenGLMaterial::~COpenGLMaterial()
	{
		Release();
	}

	void COpenGLMaterial::Release()
	{
		m_UBOList.clear();

		glDeleteProgram(m_ShaderPrg);
	}

	bool COpenGLMaterial::Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		// 参照テクスチャリスト
		if (!CreateRefTextureList(m_CreateInfo, TextureSet)) return false;

		if (!CreateShaderStages()) return false;
		if (!CreateShaderBuffers()) return false;

		return true;
	}

	bool COpenGLMaterial::ReCreate(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::vector<std::shared_ptr<graphics::CShaderBuffer>>& ShaderBufferList, const std::vector<graphics::STextureBindingLayout>& TextureBindingLayoutList)
	{
		m_CreateInfo = createInfo;

		Release();

		// 参照テクスチャリストの再生成
		if (!ReCreateRefTextureList(m_CreateInfo)) return false;

		// バッファの再生成
		if (!ReCreateBuffer(ShaderBufferList, TextureBindingLayoutList)) return false;

		if (!CreateShaderStages()) return false;
		if (!CreateShaderBuffers()) return false;

		return true;
	}

	bool COpenGLMaterial::BuildDrawBuffer(int DynamicOffsetNum)
	{
		int index = 0;
		for (const auto& Buffer : m_ShaderBufferList)
		{
			// SharedBufferは処理しない
			if (Buffer->GetSharedBufferParam().IsShared) continue;

			if (Buffer->GetBufferUpdateType() != graphics::EBufferUpdateType::UPDATE_TYPE_CPU) continue;

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
		int TexOrderIndex = 0;
		for (const auto& TexLayout : m_TextureBindingLayoutList)
		{
			std::shared_ptr<graphics::CTexture> Texture = nullptr;

			if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_2D)
			{
				const auto& it = m_RefTextureMap.find(TexLayout.TextureName);

				Texture = (it != m_RefTextureMap.end()) ? it->second : m_EmptyTexture;
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_CUBE)
			{
				const auto& it = m_RefCubeMapMap.find(TexLayout.TextureName);

				Texture = (it != m_RefCubeMapMap.end()) ? it->second : m_EmptyCubeTexture;
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_FRAME)
			{
				const auto& it = m_RefFrameTextureMap.find(TexLayout.TextureName);

				Texture = (it != m_RefFrameTextureMap.end()) ? it->second : m_EmptyTexture;
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_Diffuse)
			{
				Texture = (m_RefDiffuse_Tex) ? m_RefDiffuse_Tex : m_EmptyTexture;
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_Specular)
			{
				Texture = (m_RefSpecular_Tex) ? m_RefSpecular_Tex : m_EmptyTexture;
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_GGXLUT)
			{
				Texture = (m_RefGGXLUT_Tex) ? m_RefGGXLUT_Tex : m_EmptyTexture;
			}

			if (!Texture)
			{
				Console::Log("[ERROR] Texture is nullptr\n");
				return false;
			}

			api::COpenGLTexture* pOpenGLTexture = static_cast<api::COpenGLTexture*>(Texture.get());

			pOpenGLTexture->SetActive(GL_TEXTURE0 + TexOrderIndex);

			GLuint location = glGetUniformLocation(m_ShaderPrg, TexLayout.TextureName.c_str());
			glUniform1i(location, TexOrderIndex);

			pOpenGLTexture->SetEactive(GL_TEXTURE0 + TexOrderIndex);

			TexOrderIndex++;
		}

		return true;
	}

	void COpenGLMaterial::SetUniformValue(const std::string Name, const void* Data, int ByteSize, int DynamicOffsetNum)
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

				UniformBuffer->SetValue(Data, ByteOffset, ByteSize);
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
				GLuint blockIndex = 0;

				switch (Buffer->GetBufferType())
				{
					case graphics::EBufferType::UNIFORM:
						{
							target = GL_UNIFORM_BUFFER;
							usage = GL_STATIC_DRAW;

							// Uniformのbinding indexを割り当てる
							blockIndex = glGetUniformBlockIndex(m_ShaderPrg, Layout.second.BindingName.c_str());
							glUniformBlockBinding(m_ShaderPrg, blockIndex, Layout.second.BindingIndex); // ShaderPrgとBinding Blockを紐づける
						}
						break;

					case graphics::EBufferType::SHADERSTORAGE:
						{
							// メモ
							// なんか今、同じShaderを使いまわしている時にSSBOのbinding Indexが同じだとデータが全て一番最後のDrawのものに上書きされるような動作をしてしまう
							// OpenGLでアニメーションモデルを複数個使うことができなくなってしまうので、仕様をよく調べて将来的に修正する 

							target = GL_SHADER_STORAGE_BUFFER;
							usage = GL_DYNAMIC_DRAW;

							// binding indexを取得
							blockIndex = glGetProgramResourceIndex(m_ShaderPrg, GL_SHADER_STORAGE_BLOCK, Layout.second.BindingName.c_str());
							glShaderStorageBlockBinding(m_ShaderPrg, blockIndex, Layout.second.BindingIndex); // ShaderPrgとBinding Blockを紐づける
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

					// UBOとBinding Blockを紐づける
					glBindBufferRange(target, Layout.second.BindingIndex, sharedUboIndex, 0, Layout.second.ByteSize); 

					m_UBOList.push_back(sharedUboIndex);
				}
				else // 通常のバッファ使用
				{
					// UBOを生成
					GLuint uboIndex;
					glGenBuffers(1, &uboIndex);

					// UBOとBinding Blockを紐づける
					glBindBufferRange(target, Layout.second.BindingIndex, uboIndex, 0, Layout.second.ByteSize); 

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