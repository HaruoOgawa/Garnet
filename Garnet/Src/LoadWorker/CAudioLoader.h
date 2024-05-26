#pragma once

#include <vector>
#include <string>
#include <memory>

#include "CResource.h"

#include "CFile.h"

#include "../Audio/CAudioClip.h"

namespace resource
{
	class CAudioLoader : public resource::CResource
	{
		std::shared_ptr<audio::CAudioClip> m_TargetAudioClip;
	public:
		CAudioLoader(const std::string& FileName, const std::shared_ptr<audio::CAudioClip>& TargetAudioClip);
		virtual ~CAudioLoader();

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, const std::shared_ptr<CResourceManager>& ResourceManager) override;
	};
}