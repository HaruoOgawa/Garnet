#include "CResource.h"
#include "CLoadWorker.h"

namespace resource
{
	CResource::CResource(const std::string& FileName, int LoadPriority):
		m_Releoading(false),
		m_Status(ELoadStatus::None),
		m_File(std::make_shared<CFile>(FileName)),
		m_FileName(FileName),
		m_LoadPriority(LoadPriority),
		m_AssertedErrorMessage(std::string())
	{
	}

	CResource::~CResource()
	{
	}

	bool CResource::Load()
	{
		m_Status = resource::ELoadStatus::Loading;

		if (!m_File->Load()) return false;

		return true;
	}

	bool CResource::LoadImmediate()
	{
		return true;
	}

	bool CResource::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::IApp* pApp)
	{
		return true;
	}

	void CResource::Reset()
	{
		m_Status = ELoadStatus::None;

		if (m_File)
		{
			m_File.reset();
			m_File = nullptr;
		}

		m_File = std::make_shared<CFile>(m_FileName);

		m_AssertedErrorMessage = std::string();
	}

	bool CResource::Reload(resource::CLoadWorker* pLoadWorker)
	{
		m_Releoading = true;

		// ñ¢ÉçÅ[ÉhÇ…ñﬂÇ∑
		Reset();

		pLoadWorker->AddLoadResource(shared_from_this());

		return true;
	}

	const std::string& CResource::GetFilename() const
	{
		return m_FileName;
	}

	int CResource::GetLoadPriority() const
	{
		return m_LoadPriority;
	}

	void CResource::SetLoadStatus(resource::ELoadStatus Status)
	{
		m_Status = Status;
	}

	resource::ELoadStatus CResource::GetStatus() const
	{
		return m_Status;
	}

	bool CResource::IsLoaded() const
	{
		return (m_Status == resource::ELoadStatus::Loaded);
	}

	void CResource::AddReference(const std::shared_ptr<IResource>& Resource)
	{
		// é©êgÇÃéûÇÕí«â¡ÇµÇ»Ç¢
		if (Resource == shared_from_this())
		{
			return;
		}
	}

	const std::string& CResource::GetAssertedErrorMessage()
	{
		return m_AssertedErrorMessage;
	}
}