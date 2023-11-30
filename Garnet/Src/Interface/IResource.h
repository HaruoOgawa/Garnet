#pragma once
#include "../LoadWorker/ELoadStatus.h"

namespace resource
{
	class IResource
	{
	public:
		virtual bool Load() = 0;
		virtual bool Update() = 0;

		virtual void SetLoadStatus(resource::ELoadStatus Status) = 0;
		virtual resource::ELoadStatus GetStatus() const = 0;
		virtual bool IsLoaded() const = 0;
	};
}