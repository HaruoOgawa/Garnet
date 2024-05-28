#pragma once

#include <memory>
#include <string>
#include <json.hpp>

#include "CResource.h"

namespace scene { class CSceneController; }

namespace resource
{
	class CSceneLoader : public resource::CResource
	{
		std::shared_ptr<scene::CSceneController> m_Target;
	public:
		CSceneLoader(const std::string& FileName, const std::shared_ptr<scene::CSceneController>& Target);
		virtual ~CSceneLoader();

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, const std::shared_ptr<CResourceManager>& ResourceManager) override;
	};
}