#ifdef USE_TEXTURE_LOADER

#include "CTextureLoader.h"
#include "CLoadWorker.h"

namespace resource
{
	CTextureLoader::CTextureLoader(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::string>& FileNameList, const std::shared_ptr<graphics::CTexture>& Texture):
		CResource(""),
		m_UseCube(false),
		m_TargetTexture(Texture)
	{
		if (FileNameList.size() == 6) m_UseCube = true;

		for (const auto& filename : FileNameList)
		{
			m_FileList.push_back(std::make_shared<CFile>(filename));
		}

		if (!m_FileList.empty())
		{
			// 最初のファイルをファイル名とする
			m_FileName = m_FileList[0]->GetFilename();
		}

		Texture->SetFileNameList(FileNameList);
	}

	CTextureLoader::CTextureLoader(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, const std::shared_ptr<graphics::CTexture>& Texture) :
		CResource(FileName),
		m_UseCube(false),
		m_TargetTexture(Texture)
	{
		m_FileList.push_back(std::make_shared<CFile>(FileName));

		Texture->SetFileName(FileName);
	}

	CTextureLoader::~CTextureLoader()
	{
	}

	bool CTextureLoader::Load()
	{
		if (m_FileList.size() != 1 && m_FileList.size() != 6) return false;

		m_Status = resource::ELoadStatus::Loading;

		return true;
	}

	bool CTextureLoader::LoadImmediate()
	{
		return true;
	}

	bool CTextureLoader::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::CApp* pApp)
	{
		for (auto& Resource : m_FileList)
		{
			switch (Resource->GetStatus())
			{
			case resource::ELoadStatus::None:
				// ファイルのバイナリが実行ファイルに埋め込まれていないかチェックする
				if (pLoadWorker->FindEmbeddedBinary(pGraphicsAPI, pPhysicsEngine, Resource, pApp)) return true;

				// 通常通りロードする
				if (!Resource->Load()) return false;
				return true;

			case resource::ELoadStatus::Loading:
				if (!Resource->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, pApp)) return false;
				return true;

			case resource::ELoadStatus::Loaded:
			{
				if (m_UseCube)
				{
					m_CubeDataList.push_back(Resource->GetData());
				}
				else
				{
					m_TextureData = Resource->GetData();
				}

				// リソースマネージャーに登録
				pLoadWorker->GetResourceManager()->AddOnMemoryResource(Resource, shared_from_this());

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

		// リソースマネージャーに登録
		pLoadWorker->GetResourceManager()->AddOnMemoryResource(shared_from_this(), nullptr);

		return true;
	}
}

#endif