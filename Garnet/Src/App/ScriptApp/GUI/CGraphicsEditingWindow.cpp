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
		//const ImGuiViewport* viewport = ImGui::GetMainViewport();
		//ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y), ImGuiCond_FirstUseEver);
		//ImGui::SetNextWindowSize(ImVec2(1000, 1000), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Garnet3D"))
		{
			if (ImGui::BeginTabBar("MainMenuBar"))
			{
				if (!m_GUIObjectTab.Draw(pApp->GetObjectList())) return false;
				if (!CGUIRenderingTab::Draw()) return false;
				if (!CGUICameraTab::Draw()) return false;

				ImGui::EndTabBar();
			}

			ImGui::End();
		}

		return true;
	}
}
#endif // USE_GUIENGINE