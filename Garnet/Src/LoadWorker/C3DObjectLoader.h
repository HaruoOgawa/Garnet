#pragma once

#include <vector>
#include <map>
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
		E3DObjectLoadState m_LoadState;

		std::shared_ptr<object::C3DObject> m_TargetObject;
		const animation::ERigType		   m_TargetRigType;
		std::map<animation::EHumanoidBones, std::string> m_TargetHumanoidBoneList;

		std::vector<std::shared_ptr<graphics::CMaterialFrame>> m_BaseMaterialFrameList;

		std::vector<std::shared_ptr<resource::IResource>> m_SubResources;
	private:
		bool Import(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine);
		bool LoadSubResources(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::CApp* pApp);
	public:
		C3DObjectLoader(const std::string& FileName, const std::shared_ptr<object::C3DObject>& TargetObject, 
			const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList, const std::vector<std::string>& defaultmaterialframes,
			animation::ERigType RigType, const std::map<animation::EHumanoidBones, std::string>& HumanoidBoneList);
		virtual ~C3DObjectLoader();
		
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::CApp* pApp) override;

		void AddSubResource(const std::shared_ptr <resource::IResource>& Resource);
	};
}