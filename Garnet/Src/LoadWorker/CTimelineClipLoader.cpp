#include "CTimelineClipLoader.h"
#include "../Timeline/CTimelineClip.h"
#include "../Timeline/CTimelineImporter.h"

namespace resource
{
	CTimelineClipLoader::CTimelineClipLoader(const std::string& FileName, const std::shared_ptr<timeline::CTimelineClip>& Target):
		CResource(FileName),
		m_Target(Target)
	{
	}
	
	bool CTimelineClipLoader::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		if (!m_File->IsLoaded()) return true;

		if (!timeline::CTimelineImporter::Import(m_File->GetData(), m_Target)) return false;

		// ƒ[ƒhŠ®—¹
		m_Status = resource::ELoadStatus::Loaded;
		m_Target->SetFileName(GetFilename());

		return true;
	}
}