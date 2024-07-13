#ifdef USE_GUIENGINE
#include "CTimeLineView.h"
#include <Timeline/CTimelineController.h>

namespace gui
{
	CTimeLineView::CTimeLineView()
	{
	}

	bool CTimeLineView::Draw(const std::shared_ptr<timeline::CTimelineController>& TimelineController)
	{
		if (!TimelineController) return true;

		if (!DrawTimeBar(TimelineController)) return false;
		if (!DrawHierarchyWindow(TimelineController)) return false;
		if (!DrawKeyFrameWindow(TimelineController)) return false;

		return true;
	}

	bool CTimeLineView::DrawTimeBar(const std::shared_ptr<timeline::CTimelineController>& TimelineController)
	{
		// PlayButton
		{
			std::string Name = (TimelineController->IsPlay()) ? "Stop##Timeline" : "Play##Timeline";
			if (ImGui::Button(Name.c_str()))
			{
				TimelineController->SetPlay(!TimelineController->IsPlay());
			}
		}

		// MaxTime
		{
			// 再生ボタンと同じ位置に配置する
			ImGui::SameLine();

			float MaxTime = TimelineController->GetMaxTime();

			std::string Name = "##TimelineMaxPlaytime";
			ImGui::PushItemWidth(50.0f);
			if (ImGui::InputFloat(Name.c_str(), &MaxTime))
			{
				TimelineController->SetMaxTime(MaxTime);
			}
			ImGui::PopItemWidth();
		}

		// Playback Time Slider
		{
			// 再生ボタンと同じ位置に配置する
			ImGui::SameLine();

			float CurrentTime = TimelineController->GetCurrentTime();

			std::string Name = "##TimelinePlaybackTime";
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x); // 現在のウィンドウの利用可能な範囲までスライダーを引き延ばす. GetContentRegionAvailは現在地での利用可能なサイズを返す
			if (ImGui::SliderFloat(Name.c_str(), &CurrentTime, 0.0f, TimelineController->GetMaxTime()))
			{
				TimelineController->SetCurrentTime(CurrentTime);
			}
			ImGui::PopItemWidth();
		}

		return true;
	}

	bool CTimeLineView::DrawHierarchyWindow(const std::shared_ptr<timeline::CTimelineController>& TimelineController)
	{
		ImVec2 availableSize = ImGui::GetContentRegionAvail();

		ImGui::BeginChild("HierarchyWindow##Timeline", ImVec2(availableSize.x * 0.25f, availableSize.y));
		
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 window_pos = ImGui::GetWindowPos();
		ImVec2 window_size = ImGui::GetWindowSize();

		draw_list->AddRectFilled(window_pos, ImVec2(window_pos.x + window_size.x, window_pos.y + window_size.y), IM_COL32(255, 0, 0, 255));

		//ImGui::Button("TestButton_AAA##HierarchyWindowChild");

		ImGui::EndChild();

		return true;
	}

	bool CTimeLineView::DrawKeyFrameWindow(const std::shared_ptr<timeline::CTimelineController>& TimelineController)
	{
		ImGui::SameLine();

		ImVec2 availableSize = ImGui::GetContentRegionAvail();
		
		ImGui::BeginChild("KeyFrameWindow##Timeline", availableSize);

		// タイムラインのメモリバーを描画
		if (!DrawMemoryBar(TimelineController)) return false;

		ImGui::EndChild();

		return true;
	}

	bool CTimeLineView::DrawMemoryBar(const std::shared_ptr<timeline::CTimelineController>& TimelineController)
	{
		// タイムラインのメモリバーを描画
		ImVec2 availableSize = ImGui::GetContentRegionAvail();

		ImVec2 barSize = ImVec2(availableSize.x, 50.0f);

		// 現在のGUIの描画位置を取得(スクリーン座標系)
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		// 背景の描画
		ImGui::InvisibleButton("##TimelineMemoryBar", barSize);
		
		ImDrawList* drawList = ImGui::GetWindowDrawList(); // 描画マネージャー？ 自由に板ポリとか線とか文字を描画できるやつらしい
		drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + barSize.x, cursorPos.y + barSize.y), IM_COL32(60, 60, 60, 255)); // 矩形を描画

		// フレームごとのメモリを描画
		const float timelineLength = TimelineController->GetMaxTime();
		float frameWidth = barSize.x / timelineLength;

		for (float i = 0.0f; i < timelineLength; i++)
		{
			float x = cursorPos.x + i * frameWidth;

			if (static_cast<int>(i) % 10 == 0)
			{
				// 10フレームごとに大きいメモリを描画
				drawList->AddLine(ImVec2(x, cursorPos.y), ImVec2(x, cursorPos.y + 20.0f), IM_COL32(255, 255, 255, 255));

				std::string label = std::to_string(i);
				drawList->AddText(ImVec2(x + 2.0f, cursorPos.y + 22.0f), IM_COL32(255, 255, 255, 255), label.c_str());
			}
			else
			{
				// 通常のメモリを描画
				drawList->AddLine(ImVec2(x, cursorPos.y), ImVec2(x, cursorPos.y + 10.0f), IM_COL32(255, 255, 255, 255));
			}
		}

		// 現在のフレームを表すインジケーター
		static float CurrentFrame = 0.0f;
		float CurrentFrameX = cursorPos.x + CurrentFrame * frameWidth;
		drawList->AddLine(ImVec2(CurrentFrameX, cursorPos.y), ImVec2(CurrentFrameX, cursorPos.y + barSize.y), IM_COL32(255, 0, 0, 255));

		// タイムラインの操作
		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(0) || ImGui::IsMouseDragging(0))
			{
				ImVec2 mousePos = ImGui::GetMousePos();

				CurrentFrame = (mousePos.x - cursorPos.x) / frameWidth;
				CurrentFrame = glm::clamp(CurrentFrame, 0.0f, TimelineController->GetMaxTime());
			}
		}

		return true;
	}
}
#endif