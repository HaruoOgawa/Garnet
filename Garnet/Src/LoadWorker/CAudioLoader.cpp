#include "CAudioLoader.h"
#include "../Message/Console.h"
#include "CResourceManager.h"

namespace resource
{
	CAudioLoader::CAudioLoader(const std::string& FileName, const std::shared_ptr<audio::CAudioClip>& TargetAudioClip):
		m_Status(ELoadStatus::None),
		m_File(std::make_shared<CFile>(FileName)),
		m_FileName(FileName),
		m_TargetAudioClip(TargetAudioClip)
	{
	}

	CAudioLoader::~CAudioLoader()
	{
	}

	const std::string& CAudioLoader::GetFilename() const
	{
		return m_File->GetFilename();
	}

	void CAudioLoader::SetLoadStatus(resource::ELoadStatus Status)
	{
		m_Status = Status;
	}

	resource::ELoadStatus CAudioLoader::GetStatus() const
	{
		return m_Status;
	}
	bool CAudioLoader::IsLoaded() const
	{
		return (m_Status == resource::ELoadStatus::Loaded);
	}

	bool CAudioLoader::Load()
	{
		m_Status = resource::ELoadStatus::Loading;

		if (!m_File->Load()) return false;

		return true;
	}

	bool CAudioLoader::LoadImmediate()
	{
		return true;
	}

	bool CAudioLoader::Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager)
	{
		if (!m_File->IsLoaded())
		{
			if (!m_File->Update(pGraphicsAPI, ResourceManager)) return false;
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
		ResourceManager->AddOnMemoryResource(shared_from_this(), nullptr);

		return true;
	}
}