#pragma once
#include "../LoadWorker/ELoadStatus.h"
#include "../Interface/IGraphicsAPI.h"

namespace resource
{
	class IResource
	{
	public:
		virtual bool Load() = 0;
		virtual bool LoadImmediate() = 0;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI) = 0;

		virtual void SetLoadStatus(resource::ELoadStatus Status) = 0;
		virtual resource::ELoadStatus GetStatus() const = 0;
		virtual bool IsLoaded() const = 0;
	};
}