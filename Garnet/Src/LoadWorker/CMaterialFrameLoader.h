#pragma once

#include <vector>
#include <string>
#include <memory>
#include "../Interface/IResource.h"

namespace graphics { class CMaterialFrame; }

namespace resource
{
	class CFile;

	class CMaterialFrameLoader : public resource::IResource
	{
		resource::ELoadStatus m_Status;

		std::shared_ptr<CFile> m_MfFile;

		bool m_AnalyseDone;
		std::vector<std::shared_ptr<CFile>> m_MfResourceList;

		std::shared_ptr<graphics::CMaterialFrame> m_TargetMaterialFrame;
	private:
		bool AnalyseResourceList();
		bool CreateMaterialFrame();
	public:
		CMaterialFrameLoader(const std::string& filename, std::shared_ptr<graphics::CMaterialFrame> TargetMaterialFrame);
		virtual ~CMaterialFrameLoader();

		virtual void SetLoadStatus(resource::ELoadStatus Status) override;
		virtual resource::ELoadStatus GetStatus() const override;
		virtual bool IsLoaded() const override;

		virtual bool Load() override;
		virtual bool Update() override;
	};
}