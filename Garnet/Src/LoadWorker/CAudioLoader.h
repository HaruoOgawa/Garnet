#pragma once

#include <vector>
#include <string>
#include <memory>

#include "../Interface/IResource.h"

#include "CFile.h"

#include "../Audio/CAudioClip.h"

namespace resource
{
	class CAudioLoader : public resource::IResource
	{
		resource::ELoadStatus m_Status;

		std::shared_ptr<CFile> m_File;
		std::string m_FileName;

		std::shared_ptr<audio::CAudioClip> m_TargetAudioClip;
	public:
		CAudioLoader(const std::string& FileName, const std::shared_ptr<audio::CAudioClip>& TargetAudioClip);
		virtual ~CAudioLoader();

		virtual void SetLoadStatus(resource::ELoadStatus Status) override;
		virtual resource::ELoadStatus GetStatus() const override;
		virtual bool IsLoaded() const override;

		virtual bool Load() override;
		virtual bool LoadImmediate() override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI) override;
	};
}