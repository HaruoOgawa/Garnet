#pragma once

#include <vector>
#include <string>
#include <memory>

#include "CResource.h"

#include "../Object/C3DObject.h"

namespace resource
{
	enum class E3DObjectLoadState
	{
		None = -1,
		ImportObject,
		LoadSubResouce,
		Finish,
	};

	class C3DObjectLoader : public resource::CResource
	{
		physics::IPhysicsEngine* m_pPhysicsEngine;

		E3DObjectLoadState m_LoadState;

		std::shared_ptr<object::C3DObject> m_TargetObject;

		std::shared_ptr<graphics::CMaterialFrame> m_BaseMaterialFrame;

		std::vector<std::shared_ptr<resource::IResource>> m_SubResources;
	private:
		bool Import(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine);
		bool LoadSubResources(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager);
	public:
		C3DObjectLoader(const std::string& FileName, const std::shared_ptr<object::C3DObject>& TargetObject, const std::shared_ptr<graphics::CMaterialFrame>& BaseMaterialFrame, 
			const std::string& PassName, const std::string& DepthPassName, physics::IPhysicsEngine* pPhysicsEngine);
		virtual ~C3DObjectLoader();
		
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager) override;

		void AddSubResource(const std::shared_ptr <resource::IResource>& Resource);
	};
}