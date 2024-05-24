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
	class CLoadWorker;

	class CResource : public IResource
	{
	protected:
		bool m_Releoading;

		resource::ELoadStatus m_Status;

		std::shared_ptr<CFile> m_File;

		std::string m_FileName;

		const int m_LoadPriority;
	public:
		CResource(const std::string& FileName, int LoadPriority = 0);
		virtual ~CResource();

		virtual bool Load() override;
		virtual bool LoadImmediate() override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, const std::shared_ptr<CResourceManager>& ResourceManager) override;

		virtual void Reset() override;

		virtual bool Reload(resource::CLoadWorker* pLoadWorker) override;

		virtual const std::string& GetFilename() const override;
		virtual int GetLoadPriority() const override;

		virtual void SetLoadStatus(resource::ELoadStatus Status) override;
		virtual resource::ELoadStatus GetStatus() const override;
		virtual bool IsLoaded() const override;

		virtual void AddReference(const std::shared_ptr<IResource>& Resource) override;
	};
}