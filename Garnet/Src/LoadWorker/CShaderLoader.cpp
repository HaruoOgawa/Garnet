#include "CShaderLoader.h"
#include "CResourceManager.h"

namespace resource
{
	CShaderLoader::CShaderLoader(const std::string& FileName, const std::string& BaseFileName, const std::string& shaderType, bool autoShaderExtension):
		CResource(FileName),
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