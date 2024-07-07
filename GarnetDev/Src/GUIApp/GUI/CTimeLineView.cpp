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
}
#endif