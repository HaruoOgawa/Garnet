#pragma once

#include <string>
#include <memory>

#include "CResource.h"

#include "CFile.h"

namespace resource
{
	class CShaderLoader : public CResource
	{
		std::string m_EditingBaseFileName;
	private:
		std::vector<std::string> ExecuteCommand(const char* cmd);

		std::vector<std::string> Split(const std::string& Src, const char separate);
	public:
		CShaderLoader(const std::string& FileName, const std::string& BaseFileName, const std::string& shaderType, bool autoShaderExtension);
		virtual ~CShaderLoader();

		virtual const std::string& GetFilename() const override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager) override;

		const std::shared_ptr<CFile>& GetFile() const;

		virtual bool Reload(resource::CLoadWorker* pLoadWorker) override;
	};
}