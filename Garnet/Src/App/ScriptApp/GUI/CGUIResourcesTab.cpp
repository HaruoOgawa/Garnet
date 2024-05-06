#ifdef USE_GUIENGINE
#include "CGUIResourcesTab.h"
#include "../Model/CFileModifier.h"
#include "../../Message/Console.h"
#include <filesystem>

namespace gui
{
	CGUIResourcesTab::CGUIResourcesTab()
	{
		// ディレクトリリストを初期化
		InitDirectoryList("Resources");
	}

	CGUIResourcesTab::~CGUIResourcesTab()
	{
	}

	void CGUIResourcesTab::InitDirectoryList(std::string RootDir)
	{
		// Emscriptenはサポートしない
#ifdef __EMSCRIPTEN__
		return;
#endif // __EMSCRIPTEN__

		m_RootDir = FindDirectory(RootDir);
	}

	std::shared_ptr<SDirectoryInfo> CGUIResourcesTab::FindDirectory(const std::string& SearchDir)
	{
		std::shared_ptr<SDirectoryInfo> DstDir = std::make_shared<SDirectoryInfo>();

		DstDir->DirName = std::make_pair(DeleteParentDirName(SearchDir), SearchDir);

		for (const auto& entry : std::filesystem::directory_iterator(SearchDir))
		{
			const auto& path = entry.path().generic_string();
			std::string name = DeleteParentDirName(path);

			if (std::filesystem::is_directory(path))
			{
				DstDir->SubDirList.push_back(FindDirectory(path));
			}
			else
			{
				DstDir->FileList.push_back(std::make_pair(name, path));
			}
		}

		return DstDir;
	}

	bool CGUIResourcesTab::Draw(app::CFileModifier* pFileModifier)
	{
		// Emscriptenはサポートしない
#ifdef __EMSCRIPTEN__
		return true;
#endif // __EMSCRIPTEN__

		if (ImGui::BeginTabItem("Resources"))
		{
			if (!DrawDirGUI(m_RootDir, pFileModifier)) return false;

			ImGui::EndTabItem();
		}

		return true;
	}

	bool CGUIResourcesTab::DrawDirGUI(const std::shared_ptr<SDirectoryInfo>& Dir, app::CFileModifier* pFileModifier)
	{
		// Emscriptenはサポートしない
#ifdef __EMSCRIPTEN__
		return true;
#endif // __EMSCRIPTEN__

		const bool IsOpened = ImGui::TreeNodeEx(Dir->DirName.first.c_str(), /*ImGuiTreeNodeFlags_OpenOnDoubleClick |*/ ImGuiTreeNodeFlags_Framed);

		if (IsOpened)
		{
			// Draw SubDir
			for (const auto& SubDir : Dir->SubDirList)
			{
				if (!DrawDirGUI(SubDir, pFileModifier)) return false;
			}

			// Draw File
			for (const auto& File : Dir->FileList)
			{
				std::string Label = File.first + "##" + File.second;

				if (ImGui::Button(Label.c_str()))
				{
					std::string cmd = "start " + File.second;
					std::system(cmd.c_str());

					pFileModifier->AddEditingFileName(File.second);
				}
			}

			ImGui::TreePop();
		}

		return true;
	}

	std::vector<std::string> CGUIResourcesTab::ExecuteCommand(const char* cmd)
	{
#ifdef __EMSCRIPTEN__
		return std::vector<std::string>();
#else
		char buffer[128];
		std::string result = "";

		FILE* pipe = _popen(cmd, "r");
		if (!pipe)
		{
			Console::Log("[Error] Failed to execute command\n");
			return std::vector<std::string>();
		}

		while (fgets(buffer, sizeof buffer, pipe) != NULL)
		{
			result += buffer;
		}

		_pclose(pipe);

		return Split(result, '\n');
#endif // __EMSCRIPTEN__
		
	}

	std::vector<std::string> CGUIResourcesTab::Split(const std::string& Src, const char separate)
	{
		std::vector<std::string> TextList;

		std::string CurrentStr = Src;

		while (CurrentStr.find(separate) != -1)
		{
			int Index = static_cast<int>(CurrentStr.find(separate));

			std::string Dst = CurrentStr.substr(0, Index + 1);
			TextList.push_back(Dst);

			CurrentStr = CurrentStr.substr(Index + 1, CurrentStr.size() - Dst.size());
		}

		return TextList;
	}

	std::string CGUIResourcesTab::DeleteParentDirName(const std::string& Src)
	{
		int IndexYen = static_cast<int>(Src.rfind('\\'));
		int IndexSlash = static_cast<int>(Src.rfind('/'));

		if (IndexYen > IndexSlash)
		{
			int Index = IndexYen + 1;
			return Src.substr(Index, Src.size() - Index);
		}
		else if (IndexSlash > IndexYen)
		{
			int Index = IndexSlash + 1;
			return Src.substr(Index, Src.size() - Index);
		}
		else
		{
			return Src;
		}
	}
}
#endif