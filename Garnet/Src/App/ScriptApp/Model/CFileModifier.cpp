#include "CFileModifier.h"
#include "../../LoadWorker/CLoadWorker.h"
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

	void CFileModifier::OnFileUpdated(resource::CLoadWorker* pLoadWorker)
	{
		const std::shared_ptr<resource::CResourceManager>& ResourceManager = pLoadWorker->GetResourceManager();

		const auto& OnMemoryResourceList = ResourceManager->GetOnMemoryResourceList();

		std::map<int, std::set<std::shared_ptr<resource::IResource>>> LoadPriorityUpdateResourceMap;

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

			// リソースを更新予約リストに追加
			const int LoadPriority = it->second.ResourceData->GetLoadPriority();

			if (LoadPriorityUpdateResourceMap.find(LoadPriority) == LoadPriorityUpdateResourceMap.end())
			{
				LoadPriorityUpdateResourceMap.emplace(LoadPriority, std::set<std::shared_ptr<resource::IResource>>());
			}

			LoadPriorityUpdateResourceMap[LoadPriority].emplace(it->second.ResourceData);

			// 親リソースが存在すれば親リソースも追加する
			/*for (const auto& ParentResource : it->second.ParentResourceDataList)
			{
				ReservedUpdateResourceSet.emplace(ParentResource);
			}*/
		}

		// 更新を実行
		for (const auto& ReservedUpdateResourceSet : LoadPriorityUpdateResourceMap)
		{
			for (const auto& Resouce : ReservedUpdateResourceSet.second)
			{
				Resouce->Reload(pLoadWorker);
			}
		}

		// 予約リストをクリア
		LoadPriorityUpdateResourceMap.clear();
	}
}