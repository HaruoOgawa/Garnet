#pragma once
#include <memory>
#include "../LoadWorker/ELoadStatus.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IPhysicsEngine.h"

namespace resource
{
	class CLoadWorker;

	class IResource : public std::enable_shared_from_this<IResource>
	{
	public:
		virtual bool Load() = 0;
		virtual bool LoadImmediate() = 0;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker) = 0;

		virtual void Reset() = 0;

		virtual bool Reload(resource::CLoadWorker* pLoadWorker) = 0;

		virtual const std::string& GetFilename() const = 0;
		virtual int GetLoadPriority() const = 0;

		virtual void SetLoadStatus(resource::ELoadStatus Status) = 0;
		virtual resource::ELoadStatus GetStatus() const = 0;
		virtual bool IsLoaded() const = 0;

		virtual void AddReference(const std::shared_ptr<IResource>& Resource) = 0;

		virtual const std::string& GetAssertedErrorMessage() = 0;
	};
}