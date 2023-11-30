#include "CMaterialFrameLoader.h"

namespace resource
{
	CMaterialFrameLoader::CMaterialFrameLoader(const std::string& filename, std::shared_ptr<graphics::CMaterialFrame> TargetMaterialFrame):
		m_Status(ELoadStatus::None),
		m_MfFile(std::make_shared<CFile>(filename)),
		m_TargetMaterialFrame(TargetMaterialFrame),
		m_AnalyseDone(false),
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

	bool CMaterialFrameLoader::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_MfFile->IsLoaded()) return true;

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
				return true;
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
				{
					const auto it = shader->find("name");
					if (it != shader->end() && it->is_string())
					{
						shaderName = it.value();
					}
				}

				// shaderFile
				std::string shaderFile = "";
				{
					const auto it = shader->find("shaderFile");
					if (it != shader->end() && it->is_string())
					{
						shaderFile = it.value();
					}
				}

				// autoShaderExtension
				bool autoShaderExtension = true;
				{
					const auto it = shader->find("autoShaderExtension");
					if (it != shader->end() && it->is_boolean())
					{
						autoShaderExtension = it.value();
					}
				}

				// shadercode
				std::string shadercode = "";
				{
					const auto it = shader->find("shadercode");
					if (it != shader->end() && it->is_string())
					{
						shadercode = it.value();
					}
				}

				// shaderType
				std::string shaderType = "";
				{
					const auto it = shader->find("shaderType");
					if (it != shader->end() && it->is_string())
					{
						shaderType = it.value();
					}
				}

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

			}
		}

		return true;
	}

	bool CMaterialFrameLoader::AnalyseTextureList(api::IGraphicsAPI* pGraphicsAPI, const json::iterator& textureList)
	{
		return true;
	}

	bool CMaterialFrameLoader::CreateMaterialFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}
}