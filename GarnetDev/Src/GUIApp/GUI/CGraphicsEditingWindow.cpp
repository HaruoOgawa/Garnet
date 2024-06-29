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

	bool CGraphicsEditingWindow::Draw(api::IGraphicsAPI* pGraphicsAPI, const app::CScriptApp* pApp)
	{
		if (!pApp) return true;

		// ウィンドウの初期位置・サイズ
		float padding = 0.0f;
		ImGuiIO& io = ImGui::GetIO();

		const float MainMenuWidthRate = 0.25f;
		const float TimeLineHeightRate = 0.25f;

		// MainMenuView
		{
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - padding, padding), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
			ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * MainMenuWidthRate, io.DisplaySize.y), ImGuiCond_Always);

			bool Open = true;
			if (ImGui::Begin("MainMenuView", &Open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar))
			{
				if (ImGui::BeginTabBar("MainMenuBar"))
				{
					if (!m_GUIObjectTab.Draw(pGraphicsAPI, pApp->GetObjectList(), pApp->GetSceneController())) return false;
					if (!m_GUIResourcesTab.Draw(pApp->GetFileModifier().get())) return false;
					if (!CGUIRenderingTab::Draw()) return false;
					if (!CGUICameraTab::Draw()) return false;

					ImGui::EndTabBar();
				}
			}

			ImGui::End();
		}

		// TimeLineView
		{
			ImGui::SetNextWindowPos(ImVec2(padding, io.DisplaySize.y - padding), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
			ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * (1.0f - MainMenuWidthRate), io.DisplaySize.y * TimeLineHeightRate), ImGuiCond_Always);

			bool Open = true;
			if (ImGui::Begin("TimeLineView", &Open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar))
			{

			}

			ImGui::End();
		}

		// 3DView
		{
			ImGui::SetNextWindowPos(ImVec2(padding, io.DisplaySize.y * (1.0f - TimeLineHeightRate)), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
			ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * (1.0f - MainMenuWidthRate), io.DisplaySize.y * (1.0f - TimeLineHeightRate)), ImGuiCond_Always);

			bool Open = true;
			if (ImGui::Begin("3DView", &Open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs))
			{

			}

			ImGui::End();
		}

		return true;
	}
}
#endif // USE_GUIENGINE