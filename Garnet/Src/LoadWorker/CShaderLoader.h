#pragma once

#include <string>
#include <memory>

#include "../Interface/IResource.h"

#include "CFile.h"

namespace resource
{
	class CShaderLoader : public IResource
	{
		resource::ELoadStatus m_Status;

		std::shared_ptr<CFile> m_File;

		std::string m_FileName;
		std::string m_EditingBaseFileName;
	public:
		CShaderLoader(const std::string& FileName, const std::string& BaseFileName, const std::string& shaderType, bool autoShaderExtension);
		virtual ~CShaderLoader();

		virtual const std::string& GetFilename() const override;

		virtual void SetLoadStatus(resource::ELoadStatus Status) override;
		virtual resource::ELoadStatus GetStatus() const override;
		virtual bool IsLoaded() const override;

		virtual bool Load() override;
		virtual bool LoadImmediate() override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager) override;

		const std::shared_ptr<CFile>& GetFile() const;
	};
}