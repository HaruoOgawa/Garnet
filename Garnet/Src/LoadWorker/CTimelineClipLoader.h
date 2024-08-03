#pragma once

#include <memory>
#include <string>
#include "CResource.h"

namespace timeline { class CTimelineClip; }

namespace resource
{
	class CTimelineClipLoader : public resource::CResource
	{
		std::shared_ptr<timeline::CTimelineClip> m_Target;
	public:
		CTimelineClipLoader(const std::string& FileName, const std::shared_ptr<timeline::CTimelineClip>& Target);
		virtual ~CTimelineClipLoader() = default;

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker) override;
	};
}