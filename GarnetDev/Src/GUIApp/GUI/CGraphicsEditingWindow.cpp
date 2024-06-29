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

	bool CGraphicsEditingWindow::Draw(api::IGraphicsAPI* pGraphicsAPI, const app::CScriptApp* pApp, const std::shared_ptr<gui::IGUIEngine>& GUIEngine)
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
			if (ImGui::Begin("3DView", &Open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | 
				ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar))
			{
				auto RenderPass = pGraphicsAPI->FindOffScreenRenderPass("MRTTest");

				auto Core = GUIEngine->GetImGuiCore();

				if (RenderPass && Core)
				{
					ImVec2 WindowSize = ImGui::GetWindowSize();
					ImVec2 ImageSize = ImVec2(io.DisplaySize.x * (1.0f - TimeLineHeightRate), io.DisplaySize.y * (1.0f - TimeLineHeightRate));
					
					// 親ウィンドウの中心に配置
					ImVec2 ImagePos = ImVec2(
						(WindowSize.x - ImageSize.x) * 0.5f,
						(WindowSize.y - ImageSize.y) * 0.5f
					);
					ImGui::SetCursorPos(ImagePos);

					ImVec2 UV0 = ImVec2(0.0f, 0.0f);
					ImVec2 UV1 = ImVec2(1.0f, 1.0f);
#ifdef USE_OPENGL
					// OpenGL時は上下反転するので補正する
					UV0 = ImVec2(0.0f, 1.0f);
					UV1 = ImVec2(1.0f, 0.0f);
#endif // USE_OPENGL

					ImGui::Image(Core->CastTexID(RenderPass->GetFrameTexture().get()), ImageSize, UV0, UV1);
				}
			}

			ImGui::End();
		}

		return true;
	}
}
#endif // USE_GUIENGINE