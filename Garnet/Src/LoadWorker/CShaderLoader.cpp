#include "CShaderLoader.h"
#include "CResourceManager.h"

namespace resource
{
	CShaderLoader::CShaderLoader(const std::string& FileName, const std::string& BaseFileName, const std::string& shaderType, bool autoShaderExtension):
		m_Status(ELoadStatus::None),
		m_File(std::make_shared<CFile>(FileName)),
		m_FileName(FileName),
		m_EditingBaseFileName(BaseFileName)
	{
		if (autoShaderExtension)
		{
			if (shaderType == "vertex")
			{
				m_EditingBaseFileName += ".vert";
			}
			else if (shaderType == "fragment")
			{
				m_EditingBaseFileName += ".frag";
			}
			else if (shaderType == "compute")
			{
				m_EditingBaseFileName += ".comp";
			}
			else if (shaderType == "geometry")
			{
				m_EditingBaseFileName += ".geom";
			}
			else if (shaderType == "hull")
			{
				m_EditingBaseFileName += ".tesc";
			}
			else if (shaderType == "domain")
			{
				m_EditingBaseFileName += ".tese";
			}
		}
	}

	CShaderLoader::~CShaderLoader()
	{
	}

	const std::string& CShaderLoader::GetFilename() const
	{
		return m_EditingBaseFileName;
	}

	void CShaderLoader::SetLoadStatus(resource::ELoadStatus Status)
	{
		m_Status = Status;
	}

	resource::ELoadStatus CShaderLoader::GetStatus() const
	{
		return m_Status;
	}
	bool CShaderLoader::IsLoaded() const
	{
		return (m_Status == resource::ELoadStatus::Loaded);
	}

	bool CShaderLoader::Load()
	{
		m_Status = resource::ELoadStatus::Loading;

		if (!m_File->Load()) return false;

		return true;
	}

	bool CShaderLoader::LoadImmediate()
	{
		return true;
	}

	bool CShaderLoader::Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager)
	{
		if (!m_File->IsLoaded())
		{
			if (!m_File->Update(pGraphicsAPI, ResourceManager)) return false;
			return true;
		}

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		// リソースマネージャーに登録
		ResourceManager->AddOnMemoryResource(shared_from_this(), nullptr);

		return true;
	}

	const std::shared_ptr<CFile>& CShaderLoader::GetFile() const
	{
		return m_File;
	}
}