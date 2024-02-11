#include "CMaterialFrameLoader.h"

namespace resource
{
	CMaterialFrameLoader::CMaterialFrameLoader(const std::string& filename, const  std::shared_ptr<graphics::CMaterialFrame>& TargetMaterialFrame):
		m_Status(ELoadStatus::None),
		m_MfFile(std::make_shared<CFile>(filename)),
		m_AnalyseDone(false),
		m_TargetMaterialFrame(TargetMaterialFrame),
		m_CreateInfo(std::make_shared<graphics::CMaterialCreateInfo>())
	{
	}

	CMaterialFrameLoader::~CMaterialFrameLoader()
	{
	}

	void CMaterialFrameLoader::SetLoadStatus(resource::ELoadStatus Status)
	{
		m_Status = Status;
	}

	resource::ELoadStatus CMaterialFrameLoader::GetStatus() const
	{
		return m_Status;
	}
	bool CMaterialFrameLoader::IsLoaded() const
	{
		return (m_Status == resource::ELoadStatus::Loaded);
	}

	bool CMaterialFrameLoader::Load()
	{
		// マテリアルフレームファイルのロード
		m_Status = resource::ELoadStatus::Loading;

		if (!m_MfFile->Load()) return false;

		return true;
	}

	bool CMaterialFrameLoader::LoadImmediate()
	{
		return true;
	}

	bool CMaterialFrameLoader::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_MfFile->IsLoaded())
		{
			if (!m_MfFile->Update(pGraphicsAPI)) return false;
			return true;
		}

		// マテリアルフレームが持っているリソース一覧を取得する
		if (!m_AnalyseDone)
		{
			if (!AnalyseResourceList(pGraphicsAPI)) return false;

			m_AnalyseDone = true;

			return true;
		}

		// マテリアルフレームが持っているリソース一覧を読む
		for (const auto& Resource : m_MfResourceList)
		{
			switch (Resource->GetStatus())
			{
			case resource::ELoadStatus::None:
				if (!Resource->Load()) return false;
				return true;

			case resource::ELoadStatus::Loading:
				if (!Resource->Update(pGraphicsAPI)) return false;
				return true;

			case resource::ELoadStatus::Loaded:
			{
				m_MfResourceList.erase(m_MfResourceList.begin());
			}
			return true;

			default:
				break;
			}
		}

		// マテリアルフレームを作成
		if (!CreateMaterialFrame(pGraphicsAPI)) return false;

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		return true;
	}

	bool CMaterialFrameLoader::AnalyseResourceList(api::IGraphicsAPI* pGraphicsAPI)
	{
		std::string RawData = std::string();
		RawData.resize(m_MfFile->GetData().size());
		std::memcpy(&RawData[0], &m_MfFile->GetData()[0], m_MfFile->GetData().size());

		m_MfJson = json::parse(RawData.c_str());

		// shaderList
		const auto shaderList = m_MfJson.find("shaderList");
		if (shaderList != m_MfJson.end() && shaderList->is_array())
		{
			if (!AnalyseShaderList(pGraphicsAPI, shaderList)) return false;
		}
		
		// textureList
		const auto textureList = m_MfJson.find("textureList");
		if (textureList != m_MfJson.end() && textureList->is_array())
		{
			if (!AnalyseTextureList(pGraphicsAPI, textureList)) return false;
		}

		return true;
	}

	bool CMaterialFrameLoader::AnalyseShaderList(api::IGraphicsAPI* pGraphicsAPI, const json::iterator& shaderList)
	{
		for (json::iterator shader = shaderList->begin(); shader != shaderList->end(); ++shader)
		{
			if (shader->is_object())
			{
				// name
				std::string shaderName = "";
				GetString("name", shaderName, shader);

				// shaderFile
				std::string shaderFile = "";
				GetString("shaderFile", shaderFile, shader);

				// autoShaderExtension
				bool autoShaderExtension = true;
				GetBoolean("autoShaderExtension", autoShaderExtension, shader);

				// shadercode
				std::string shadercode = "";
				GetString("shadercode", shadercode, shader);

				// shaderType
				std::string shaderType = "";
				GetString("shaderType", shaderType, shader);

				if (!shadercode.empty())
				{
					std::vector<unsigned char> ShaderCodeArray;
					ShaderCodeArray.resize(shadercode.length());
					std::memcpy(&ShaderCodeArray[0], &shadercode[0], shadercode.size());

					// ShaderCodeが直接書かれているのでそのままCreateInfoに渡す
					if (shaderType == "vertex")
					{
						m_CreateInfo->SetVertexShaderCode(ShaderCodeArray);
					}
					else if (shaderType == "fragment")
					{
						m_CreateInfo->SetFragmentShaderCode(ShaderCodeArray);
					}
					else if (shaderType == "compute")
					{
						m_CreateInfo->SetComputeShaderCode(ShaderCodeArray);
					}
					else if (shaderType == "geometry")
					{
						m_CreateInfo->SetGeometryShaderCode(ShaderCodeArray);
					}
					else if (shaderType == "hull")
					{
						m_CreateInfo->SetHullShaderCode(ShaderCodeArray);
					}
					else if (shaderType == "domain")
					{
						m_CreateInfo->SetDomainShaderCode(ShaderCodeArray);
					}
				}
				else
				{
					// ロードファイルリストに入れてファイルをロードする
					std::string fullshaderFilePath = shaderFile;

					// OpenGL・Vulkan・WebGPUに沿った拡張子を自動でつける
					// ファイル名のルールに従う必要がある
					if (autoShaderExtension)
					{
						if (shaderType == "vertex")
						{
							fullshaderFilePath += pGraphicsAPI->GetVertexShaderExtension();
						}
						else if (shaderType == "fragment")
						{
							fullshaderFilePath += pGraphicsAPI->GetFragmentShaderExtension();
						}
						else if (shaderType == "compute")
						{
							fullshaderFilePath += pGraphicsAPI->GetComputeShaderExtension();
						}
						else if (shaderType == "geometry")
						{
							fullshaderFilePath += pGraphicsAPI->GetGeometryShaderExtension();
						}
						else if (shaderType == "hull")
						{
							fullshaderFilePath += pGraphicsAPI->GetHullShaderExtension();
						}
						else if (shaderType == "domain")
						{
							fullshaderFilePath += pGraphicsAPI->GetDomainShaderExtension();
						}
					}

					// リソースを追加
					std::shared_ptr<CFile> LoadShaderFile = std::make_shared<CFile>(fullshaderFilePath);

					m_MfResourceList.push_back(LoadShaderFile);
					m_ShaderFileList.emplace(shaderType, LoadShaderFile);
				}

				// uniformBlockList
				const auto uniformBlockList = shader->find("uniformBlockList");
				for (json::iterator uniform = uniformBlockList->begin(); uniform != uniformBlockList->end(); uniform++)
				{
					std::string uniform_type = "";
					GetString("type", uniform_type, uniform);

					if (uniform_type == "ubo" || uniform_type == "ssbo")
					{
						if (!AnalyseShaderBuffer(uniform, uniform_type)) return false;
					}
					else if (uniform_type == "texture")
					{
						if (!AnalyseTextureBuffer(uniform)) return false;
					}
				}
			}
		}

		return true;
	}

	bool CMaterialFrameLoader::AnalyseShaderBuffer(const json::iterator& uniform, const std::string& uniform_type)
	{
		// 取得
		std::string name = "";
		GetString("name", name, uniform);

		int binding = -1;
		GetInt("binding", binding, uniform);

		bool isWritable = false;
		GetBoolean("isWritable", isWritable, uniform);

		// BindingLayoutを作成
		graphics::SBindingLayout BindingLayout = { name , binding, isWritable };

		std::string bufferUpdateType = "";
		GetString("bufferUpdateType", bufferUpdateType, uniform);

		if (bufferUpdateType.empty() || bufferUpdateType == "cpu")
		{
			BindingLayout.BufferUpdateType = graphics::EBufferUpdateType::UPDATE_TYPE_CPU;
		}
		else if (bufferUpdateType == "gpu")
		{
			BindingLayout.BufferUpdateType = graphics::EBufferUpdateType::UPDATE_TYPE_GPU;
		}

		// BufferValueを取得
		std::vector<std::shared_ptr<graphics::SBufferValueLayout>> ValueLayoutList;

		const auto values = uniform->find("values");
		if (values != uniform->end() && values->is_array())
		{
			for (json::iterator val = values->begin(); val != values->end(); val++)
			{
				std::string value_name = "";
				GetString("name", value_name, val);

				std::string value_type = "";
				GetString("type", value_type, val);

				int ByteSize = 0;
				if (value_type == "mat4")
				{
					ByteSize = sizeof(glm::mat4);
				}
				else if (value_type == "mat3")
				{
					ByteSize = sizeof(glm::mat3);
				}
				else if (value_type == "mat2")
				{
					ByteSize = sizeof(glm::mat2);
				}
				else if (value_type == "vec4")
				{
					ByteSize = sizeof(glm::vec4);
				}
				else if (value_type == "vec3")
				{
					ByteSize = sizeof(glm::vec3);
				}
				else if (value_type == "vec2")
				{
					ByteSize = sizeof(glm::vec2);
				}
				else if (value_type == "float")
				{
					ByteSize = sizeof(float);
				}
				else if (value_type == "int")
				{
					ByteSize = sizeof(int);
				}

				// ひとまずfloatArrayにしておく. floatもintもバイトサイズは同じなので
				std::vector<float> initValue;
				GetArray32("initValue", initValue, val);

				std::shared_ptr<graphics::SBufferValueLayout> ValueLayout = std::make_shared<graphics::SBufferValueLayout>();
				ValueLayout->Name = value_name;
				ValueLayout->Data = initValue;
				ValueLayout->ByteSize = ByteSize;
				ValueLayout->BindingIndex = binding;

				ValueLayoutList.push_back(ValueLayout);
			}
		}

		if (uniform_type == "ubo")
		{
			m_ShaderBufferList.push_back({ graphics::EBufferType::UNIFORM, BindingLayout, ValueLayoutList });
		}
		else if (uniform_type == "ssbo")
		{
			m_ShaderBufferList.push_back({ graphics::EBufferType::SHADERSTORAGE, BindingLayout, ValueLayoutList });
		}

		return true;
	}

	bool CMaterialFrameLoader::AnalyseTextureBuffer(const json::iterator& uniform)
	{
		std::string name = "";
		GetString("name", name, uniform);

		int viewBinding = -1;
		GetInt("viewBinding", viewBinding, uniform);

		int samplerBinding = -1;
		GetInt("samplerBinding", samplerBinding, uniform);

		int textureIndex = -1;
		GetInt("textureIndex", textureIndex, uniform);

		std::string type = "";
		GetString("type", type, uniform);

		std::string textureUsage_str = "";
		GetString("textureUsage", textureUsage_str, uniform);

		graphics::ETextureUsage TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_2D;
		
		if (textureUsage_str == "2d")
		{
			TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_2D;
		}
		else if (textureUsage_str == "cube")
		{
			TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_CUBE;
		}
		else if (textureUsage_str == "frame")
		{
			TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_FRAME;
		}
		else if (textureUsage_str == "diffuse")
		{
			TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_IBL_Diffuse;
		}
		else if (textureUsage_str == "specular")
		{
			TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_IBL_Specular;
		}
		else if (textureUsage_str == "ggx")
		{
			TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_IBL_GGXLUT;
		}

		// TextureBindingLayoutを作成
		graphics::STextureBindingLayout BindingLayout = { name , viewBinding, samplerBinding, textureIndex,TextureUsage };

		// 配列に登録
		m_TextureBufferList.push_back(BindingLayout);

		return true;
	}

	bool CMaterialFrameLoader::AnalyseTextureList(api::IGraphicsAPI* pGraphicsAPI, const json::iterator& textureList)
	{
		// あまりメリットがないので未実装
		// C3DObjectのTetureListは他のマテリアルでも共有しており、結局ReplaceTextureIndexでインデックスを調整することになるので無駄である

		return true;
	}

	bool CMaterialFrameLoader::CreateMaterialFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		// ロードが必要だったShaderリストを登録する
		for (const auto& ShaderFile : m_ShaderFileList)
		{
			const auto& shaderType = ShaderFile.first;

			std::vector<unsigned char> ShaderCodeArray = ShaderFile.second->GetData();

			// ShaderCodeが直接書かれているのでそのままCreateInfoに渡す
			if (shaderType == "vertex")
			{
				m_CreateInfo->SetVertexShaderCode(ShaderCodeArray);
			}
			else if (shaderType == "fragment")
			{
				m_CreateInfo->SetFragmentShaderCode(ShaderCodeArray);
			}
			else if (shaderType == "compute")
			{
				m_CreateInfo->SetComputeShaderCode(ShaderCodeArray);
			}
			else if (shaderType == "geometry")
			{
				m_CreateInfo->SetGeometryShaderCode(ShaderCodeArray);
			}
			else if (shaderType == "hull")
			{
				m_CreateInfo->SetHullShaderCode(ShaderCodeArray);
			}
			else if (shaderType == "domain")
			{
				m_CreateInfo->SetDomainShaderCode(ShaderCodeArray);
			}
		}

		// ロードが必要だったテクスチャリストを登録する

		// MaterialFrameを生成
		if (m_TargetMaterialFrame)
		{
			m_TargetMaterialFrame->SetCreateInfo(m_CreateInfo);
			m_TargetMaterialFrame->SetShaderBufferList(m_ShaderBufferList);
			m_TargetMaterialFrame->SetTextureBufferList(m_TextureBufferList);
		}

		return true;
	}

	void CMaterialFrameLoader::GetString(const std::string& Key, std::string& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_string())
		{
			Value = it.value();
		}
	}

	void CMaterialFrameLoader::GetBoolean(const std::string& Key, bool& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_boolean())
		{
			Value = it.value();
		}
	}

	void CMaterialFrameLoader::GetInt(const std::string& Key, int& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_number_integer())
		{
			Value = it.value();
		}
	}

	void CMaterialFrameLoader::GetFloat(const std::string& Key, float& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_number_float())
		{
			Value = it.value();
		}
	}

	void CMaterialFrameLoader::GetArray32(const std::string& Key, std::vector<float>& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_array())
		{
			for (json::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				if (it2->is_number())
				{
					float val = it2.value();

					Value.push_back(val);
				}
			}
		}
	}
}