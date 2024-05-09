#pragma once
#include <string>
#include <memory>
#include "CFile.h"
#include "../LoadWorker/ELoadStatus.h"
#include "../Interface/IResource.h"
#include "../Interface/IGraphicsAPI.h"

namespace resource
{
	class CResourceManager;

	class CResource : public IResource
	{
	protected:
		resource::ELoadStatus m_Status;

		std::shared_ptr<CFile> m_File;

		std::string m_FileName;
	public:
		CResource(const std::string& FileName);
		virtual ~CResource();

		virtual bool Load();
		virtual bool LoadImmediate();
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager) = 0;

		virtual const std::string& GetFilename() const;

		virtual void SetLoadStatus(resource::ELoadStatus Status);
		virtual resource::ELoadStatus GetStatus() const;
		virtual bool IsLoaded() const;
	};
}