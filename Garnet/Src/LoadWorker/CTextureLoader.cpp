#ifdef USE_TEXTURE_LOADER

#include "CTextureLoader.h"

namespace resource
{
	CTextureLoader::CTextureLoader(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::string>& FileNameList, const std::shared_ptr<graphics::CTexture>& Texture):
		m_Status(ELoadStatus::None),
		m_UseCube(false),
		m_TargetTexture(Texture)
	{
		if (FileNameList.size() == 6) m_UseCube = true;

		for (const auto& filename : FileNameList)
		{
			m_FileList.push_back(std::make_shared<CFile>(filename));
		}
	}

	CTextureLoader::CTextureLoader(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, const std::shared_ptr<graphics::CTexture>& Texture) :
		m_Status(ELoadStatus::None),
		m_UseCube(false),
		m_TargetTexture(Texture)
	{
		m_FileList.push_back(std::make_shared<CFile>(FileName));
	}
	
	CTextureLoader::CTextureLoader(api::IGraphicsAPI* pGraphicsAPI, const std::wstring& FileName, const std::shared_ptr<graphics::CTexture>& Texture) :
		m_Status(ELoadStatus::None),
		m_UseCube(false),
		m_TargetTexture(Texture)
	{
		m_FileList.push_back(std::make_shared<CFile>(FileName));
	}

	CTextureLoader::~CTextureLoader()
	{
	}

	void CTextureLoader::SetLoadStatus(resource::ELoadStatus Status)
	{
		m_Status = Status;
	}

	resource::ELoadStatus CTextureLoader::GetStatus() const
	{
		return m_Status;
	}

	bool CTextureLoader::IsLoaded() const
	{
		return (m_Status == resource::ELoadStatus::Loaded);
	}

	bool CTextureLoader::Load()
	{
		if (m_FileList.size() != 1 && m_FileList.size() != 6) return false;

		// マテリアルフレームファイルのロード
		m_Status = resource::ELoadStatus::Loading;

		return true;
	}
	bool CTextureLoader::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		for (const auto& File : m_FileList)
		{
			switch (File->GetStatus())
			{
			case resource::ELoadStatus::None:
				if (!File->Load()) return false;
				return true;

			case resource::ELoadStatus::Loading:
				if (!File->Update(pGraphicsAPI)) return false;
				return true;

			case resource::ELoadStatus::Loaded:
			{
				if (m_UseCube)
				{
					m_CubeDataList.push_back(File->GetData());
				}
				else
				{
					m_TextureData = File->GetData();
				}

				m_FileList.erase(m_FileList.begin());
			}
			return true;

			default:
				break;
			}
		}

		// テクスチャのロード
		if (m_UseCube)
		{
			if (!m_TargetTexture->Create(m_CubeDataList)) return false;
		}
		else
		{
			if (!m_TargetTexture->Create(m_TextureData)) return false;
		}

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		return true;
	}
}

#endif