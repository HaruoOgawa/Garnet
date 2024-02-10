#pragma once

#include <vector>
#include <string>
#include <memory>

#include "../Interface/IResource.h"

#include "CFile.h"
#include "../Object/C3DObject.h"

namespace resource
{
	class C3DObjectLoader : public resource::IResource
	{
		resource::ELoadStatus m_Status;

		std::shared_ptr<CFile> m_File;
		std::string m_FileName;

		std::shared_ptr<object::C3DObject> m_TargetObject;

	public:
		C3DObjectLoader(const std::string& FileName, const std::shared_ptr<object::C3DObject>& TargetObject, const std::string& PassName, const std::string& DepthPassName);
		virtual ~C3DObjectLoader();

		virtual void SetLoadStatus(resource::ELoadStatus Status) override;
		virtual resource::ELoadStatus GetStatus() const override;
		virtual bool IsLoaded() const override;

		virtual bool Load() override;
		virtual bool LoadImmediate() override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI) override;
	};
}