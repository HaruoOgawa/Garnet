#include "CMaterialFrameLoader.h"
#include "CFile.h"

#include "../Graphics/CMaterialFrame.h"

namespace resource
{
	CMaterialFrameLoader::CMaterialFrameLoader(const std::string& filename, std::shared_ptr<graphics::CMaterialFrame> TargetMaterialFrame):
		m_Status(ELoadStatus::None),
		m_MfFile(std::make_shared<CFile>(filename)),
		m_TargetMaterialFrame(TargetMaterialFrame),
		m_AnalyseDone(false)
	{
	}

	CMaterialFrameLoader::~CMaterialFrameLoader()
	{
	}

	void CMaterialFrameLoader::SetLoadStatus(resource::ELoadStatus Status)
	{
		m_Status = Status;
	}

	resource::ELoadStatus CMaterialFrameLoader::GetStatus() const
	{
		return m_Status;
	}
	bool CMaterialFrameLoader::IsLoaded() const
	{
		return (m_Status == resource::ELoadStatus::Loaded);
	}

	bool CMaterialFrameLoader::Load()
	{
		// マテリアルフレームファイルのロード
		m_Status = resource::ELoadStatus::Loading;

		if (!m_MfFile->Load()) return false;

		return true;
	}

	bool CMaterialFrameLoader::Update()
	{
		if (!m_MfFile->IsLoaded()) return true;

		// マテリアルフレームが持っているリソース一覧を取得する
		if (!m_AnalyseDone)
		{
			if (!AnalyseResourceList()) return false;

			m_AnalyseDone = true;

			return true;
		}

		// マテリアルフレームが持っているリソース一覧を読む
		for (const auto& Resource : m_MfResourceList)
		{
			switch (Resource->GetStatus())
			{
			case resource::ELoadStatus::None:
				if (!Resource->Load()) return false;
				return true;

			case resource::ELoadStatus::Loading:
				if (!Resource->Update()) return false;
				return true;

			case resource::ELoadStatus::Loaded:
			{
				m_MfResourceList.erase(m_MfResourceList.begin());
				m_MfResourceList.shrink_to_fit();
			}
			return true;

			default:
				return true;
			}
		}

		// マテリアルフレームを作成
		if (!CreateMaterialFrame()) return false;

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		return true;
	}

	bool CMaterialFrameLoader::AnalyseResourceList()
	{
		return true;
	}

	bool CMaterialFrameLoader::CreateMaterialFrame()
	{
		return true;
	}
}