#include "CFileModifier.h"
#include "../../LoadWorker/CResourceManager.h"

namespace app
{
	CFileModifier::CFileModifier()
	{
	}

	void CFileModifier::AddEditingFileName(const std::string& FileName)
	{
		m_EditingFileNameSet.emplace(FileName);
	}

	void CFileModifier::OnFileUpdated(const std::shared_ptr<resource::CResourceManager>& ResourceManager)
	{
		const auto& OnMemoryResourceList = ResourceManager->GetOnMemoryResourceList();

		for (const auto& EditingFileName : m_EditingFileNameSet)
		{
			// ファイルが存在するかチェック
			const auto& it = OnMemoryResourceList.find(EditingFileName);
			if (it == OnMemoryResourceList.end()) continue;

			// 最終編集時刻が変わっていないかチェック
			const auto PrevEditTime = it->second.FinalEditTime;
			const auto FinalEditTime = std::filesystem::last_write_time(EditingFileName);

			if (PrevEditTime == FinalEditTime)
			{
				// 同じなのでファイルが編集されていない
				continue;
			}

			// ファイルが編集されているので再コンパイル・更新処理を行う必要がある
			// 最終編集時刻を更新
			ResourceManager->UpdateFinalEditTime(EditingFileName, FinalEditTime);

			// リソースの更新
			const auto& Resouce = it->second.ResourceData;
		}
	}
}