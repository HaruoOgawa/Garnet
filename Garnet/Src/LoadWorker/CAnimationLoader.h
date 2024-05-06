#pragma once
#ifdef USE_ANIMATION
#include <vector>
#include <string>
#include <memory>

#include "../Interface/IResource.h"

#include "CFile.h"
#include "../Animation/CAnimationClipSet.h"

namespace resource
{
	class CAnimationLoader : public resource::IResource
	{
		resource::ELoadStatus m_Status;

		std::shared_ptr<CFile> m_File;
		std::string m_FileName;

		std::shared_ptr<animation::CAnimationClipSet> m_Target;
	public:
		CAnimationLoader(const std::string& FileName, const std::shared_ptr<animation::CAnimationClipSet>& Target);
		virtual ~CAnimationLoader();

		virtual const std::string& GetFilename() const override;

		virtual void SetLoadStatus(resource::ELoadStatus Status) override;
		virtual resource::ELoadStatus GetStatus() const override;
		virtual bool IsLoaded() const override;

		virtual bool Load() override;
		virtual bool LoadImmediate() override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager) override;
	};
}
#endif