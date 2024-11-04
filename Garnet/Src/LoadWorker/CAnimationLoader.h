#pragma once
#ifdef USE_ANIMATION
#include <vector>
#include <string>
#include <memory>

#include "CResource.h"

#include "CFile.h"
#include "../Animation/CAnimationClipSet.h"

namespace resource
{
	class CAnimationLoader : public resource::CResource
	{
		std::shared_ptr<animation::CAnimationClipSet> m_Target;
		const animation::ERigType		   m_TargetRigType;
	public:
		CAnimationLoader(const std::string& FileName, const std::shared_ptr<animation::CAnimationClipSet>& Target, animation::ERigType RigType);
		virtual ~CAnimationLoader();

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::CApp* pApp) override;
	};
}
#endif