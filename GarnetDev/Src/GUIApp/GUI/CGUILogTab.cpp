#ifdef USE_GUIENGINE
#include "CGUILogTab.h"
//#include <iostream>
//#include <chrono>
//#include <iomanip>
//#include <sstream>
//#include <ctime>

namespace gui
{
	CGUILogTab::CGUILogTab():
		m_ShowErrorDialog(false),
		m_ErrorDialogMsg(std::string())
	{
	}

	bool CGUILogTab::Draw(api::IGraphicsAPI* pGraphicsAPI, const SGUIParams& GUIParams)
	{
		if (ImGui::BeginTabItem("Log"))
		{
			if (!DrawLogList()) return false;

			ImGui::EndTabItem();
		}

		if (m_ShowErrorDialog)
		{
			if (!DrawErrorDialog()) return false;
		}

		return true;
	}

	void CGUILogTab::AddLog(gui::EGUILogType LogType, const std::string Msg)
	{
		// 追加
		m_LogList.push_back(std::make_tuple(LogType, Msg));

		// エラーメッセージを受け取ったならダイアログに表示する
		if (LogType == gui::EGUILogType::Error)
		{
			m_ShowErrorDialog = true;
			m_ErrorDialogMsg = Msg;
		}
	}

	bool CGUILogTab::DrawLogList()
	{
		return true;
	}

	bool CGUILogTab::DrawErrorDialog()
	{
		ImGuiIO& io = ImGui::GetIO();

		ImVec2 WindowSize = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(WindowSize, ImGuiCond_Appearing);

		if (ImGui::Begin("ErrorDialog##CGUILogTab_DrawErrorDialog", &m_ShowErrorDialog))
		{
			//std::string CurrentTime = GetCurrentTime();
			//ImGui::Text(CurrentTime.c_str());

			//
			ImVec4 textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

			ImGui::PushStyleColor(ImGuiCol_Text, textColor);
			ImGui::Text(m_ErrorDialogMsg.c_str());
			ImGui::PopStyleColor();
		}

		ImGui::End();

		return true;
	}

	/*std::string CGUILogTab::GetCurrentTime()
	{
		auto now = std::chrono::system_clock::now();

		std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

		std::tm* now_tm = std::localtime(&now_time_t);

		std::stringstream ss;
		ss << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");

		return ss.str();
	}*/
}
#endif