#include "CAudioLoader.h"
#include "../Message/Console.h"
#include "CLoadWorker.h"

namespace resource
{
	CAudioLoader::CAudioLoader(const std::string& FileName, const std::shared_ptr<audio::CAudioClip>& TargetAudioClip):
		CResource(FileName),
		m_TargetAudioClip(TargetAudioClip)
	{
	}

	CAudioLoader::~CAudioLoader()
	{
	}

	bool CAudioLoader::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		if (!m_File->IsLoaded())
		{
			if (!m_File->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker)) return false;
			return true;
		}

		const auto& Extention = m_File->GetExtention();

		audio::EAudioType AudioType = audio::EAudioType::none;

		if (Extention == "wav")
		{
			AudioType = audio::EAudioType::wav;
		}
		else if (Extention == "mp3")
		{
			AudioType = audio::EAudioType::mp3;
		}
		else
		{
			Console::Log("[Error - AudioLoader] Unsupported file extention: %s\n", Extention.c_str());

			return false;
		}

		if (!m_TargetAudioClip->Create(m_File->GetData(), m_FileName)) return false;

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		// リソースマネージャーに登録
		pLoadWorker->GetResourceManager()->AddOnMemoryResource(shared_from_this(), nullptr);

		return true;
	}
}