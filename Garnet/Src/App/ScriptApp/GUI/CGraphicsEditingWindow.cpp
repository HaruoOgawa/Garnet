#include "CGraphicsEditingWindow.h"
#include "../../Message/Console.h"

#ifdef USE_GUIENGINE
namespace gui
{
	CGraphicsEditingWindow::CGraphicsEditingWindow()
	{
	}

	CGraphicsEditingWindow::~CGraphicsEditingWindow()
	{
	}

	bool CGraphicsEditingWindow::Draw(const app::IApp* pApp)
	{
		if (!pApp) return true;

		// ウィンドウの初期位置・サイズ
		float padding = 0.0f;
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - padding, padding), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.25f, io.DisplaySize.y), ImGuiCond_Always);

		bool Open = true;
		if (ImGui::Begin("Garnet3D", &Open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
		{
			if (ImGui::BeginTabBar("MainMenuBar"))
			{
				if (!m_GUIObjectTab.Draw(pApp->GetObjectList())) return false;
				if (!CGUIRenderingTab::Draw()) return false;
				if (!CGUICameraTab::Draw()) return false;

				ImGui::EndTabBar();
			}
		}

		ImGui::End();

		return true;
	}
}
#endif // USE_GUIENGINE