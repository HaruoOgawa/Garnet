#pragma once

#include <vector>
#include <string>
#include <memory>

#include "../Interface/IResource.h"

#include "CFile.h"
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

	class C3DObjectLoader : public resource::IResource
	{
		physics::IPhysicsEngine* m_pPhysicsEngine;

		resource::ELoadStatus m_Status;

		E3DObjectLoadState m_LoadState;

		std::shared_ptr<CFile> m_File;
		std::string m_FileName;

		std::shared_ptr<object::C3DObject> m_TargetObject;

		std::shared_ptr<graphics::CMaterialFrame> m_BaseMaterialFrame;

		std::vector<std::shared_ptr<resource::IResource>> m_SubResources;
	private:
		bool Import(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine);
		bool LoadSubResources(api::IGraphicsAPI* pGraphicsAPI);
	public:
		C3DObjectLoader(const std::string& FileName, const std::shared_ptr<object::C3DObject>& TargetObject, const std::shared_ptr<graphics::CMaterialFrame>& BaseMaterialFrame, 
			const std::string& PassName, const std::string& DepthPassName, physics::IPhysicsEngine* pPhysicsEngine);
		virtual ~C3DObjectLoader();

		virtual void SetLoadStatus(resource::ELoadStatus Status) override;
		virtual resource::ELoadStatus GetStatus() const override;
		virtual bool IsLoaded() const override;

		virtual bool Load() override;
		virtual bool LoadImmediate() override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI) override;

		void AddSubResource(const std::shared_ptr <resource::IResource>& Resource);
	};
}