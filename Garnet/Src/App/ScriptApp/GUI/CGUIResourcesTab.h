#pragma once

#ifdef USE_GUIENGINE
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <imgui.h>

namespace app { class CFileModifier; }

namespace gui
{
	struct SDirectoryInfo
	{
		std::pair<std::string, std::string> DirName;

		std::vector<std::shared_ptr<SDirectoryInfo>> SubDirList;

		std::vector<std::pair<std::string, std::string>> FileList;
	};

	class CGUIResourcesTab
	{
		std::shared_ptr<SDirectoryInfo> m_RootDir;

		std::set<std::string> m_EditingFileNameList;
	private:
		void InitDirectoryList(std::string RootDir);
		std::shared_ptr<SDirectoryInfo> FindDirectory(const std::string& SearchDir);

		bool DrawDirGUI(const std::shared_ptr<SDirectoryInfo>& Dir, app::CFileModifier* pFileModifier);

		std::vector<std::string> ExecuteCommand(const char* cmd);

		std::vector<std::string> Split(const std::string& Src, const char separate);

		std::string DeleteParentDirName(const std::string& Src);
		
	public:
		CGUIResourcesTab();
		virtual ~CGUIResourcesTab();

		bool Draw(app::CFileModifier* pFileModifier);
	};
}
#endif