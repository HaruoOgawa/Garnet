#include "CResourceManager.h"

namespace resource
{
	CResourceManager::CResourceManager()
	{
	}

	CResourceManager::~CResourceManager()
	{
	}

	void CResourceManager::AddOnMemoryResource(const std::string& Path, const std::shared_ptr<IResource>& Resouce)
	{
		if (m_OnMemoryResourceList.find(Path) != m_OnMemoryResourceList.end()) return;

		m_OnMemoryResourceList.emplace(Path, Resouce);
	}

	void CResourceManager::AddEditingResource(const std::string& Path)
	{
		// 既に編集中リソースリストに追加されているのならスキップ
		if (m_EditingResourceList.find(Path) != m_EditingResourceList.end()) return;

		// リソースがメモリ上に展開されていない(未ロード)ならスキップ
		const auto& it = m_OnMemoryResourceList.find(Path);
		if (it == m_OnMemoryResourceList.end()) return;

		// 追加
		m_EditingResourceList.emplace(Path, it->second);
	}

	bool CResourceManager::RecreateIfEdited()
	{
		for (const auto& Resouce : m_EditingResourceList)
		{
			// 最終編集日時をチェックして更新されていれば再作成する
		}

		// 既に閉じられていれば編集中のリソースリストから削除する

		return true;
	}
}