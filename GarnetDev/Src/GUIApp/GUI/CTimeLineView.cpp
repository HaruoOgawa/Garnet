#ifdef USE_GUIENGINE
#include "CTimeLineView.h"
#include <Timeline/CTimelineController.h>
#include <Message/Console.h>

namespace gui
{
	CTimeLineView::CTimeLineView():
		m_LargeMemoryWidth(1.0f),
		m_MemoryExpandRate(0.0f),
		m_FirstClicked(true),
		m_PrevMousePos(ImVec2(0.0f, 0.0f)),
		m_ClickedIndicator(false),
		m_IndicatorRate(0.0f)
	{
		m_LeftSideMemory = 0.0f;
		m_RightSideMemory = static_cast<float>(m_MaxLargeMemoryCount) * m_LargeMemoryWidth;
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

		ImVec2 barSize = ImVec2(availableSize.x, 40.0f);

		// メモリとメモリの間隔
		const float DrawMemorySpace = availableSize.x / (m_MaxLargeMemoryCount * 3);

		ImDrawList* drawList = ImGui::GetWindowDrawList(); // 描画マネージャー？ 自由に板ポリとか線とか文字を描画できるやつらしい

		// 現在のGUIの描画位置を取得(スクリーン座標系)
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		// インジケーターの前計算。必ずメモリバーよりも先に計算しておく必要がある
		if (!CalcIndicator(cursorPos, barSize)) return false;

		// 背景の描画
		ImGui::SetCursorScreenPos(cursorPos);
		ImGui::InvisibleButton("##TimelineMemoryBar", barSize);

		// Item(ここではInvisibleButton)にホバーしているかを見たりするので必ずこの後にマウスホイールやドラッグをチェックする
		if (!CheckWheelExpand()) return false;
		if (!CheckMemoryDrag(availableSize, DrawMemorySpace, TimelineController->GetMaxTime())) return false;

		drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + barSize.x, cursorPos.y + barSize.y), IM_COL32(60, 60, 60, 255)); // 矩形を描画

		// メモリの開始値
		std::vector<bool> IsLongMemory;
		float LargeMemoryValue = GetFirstLargeMemory(m_LeftSideMemory, IsLongMemory);
		const float FirstMemoryValue = GetFirstMemory(m_LeftSideMemory);

		float MemoryOffset = (FirstMemoryValue - m_LeftSideMemory) * DrawMemorySpace;

		// メモリの描画(拡大時に隙間が見えないようにいくつか余分に描画)
		for (int i = 0; i < (m_MaxLargeMemoryCount * 3 + 4); i++)
		{
			int LoopCounter = i % 3;

			float x = cursorPos.x + static_cast<float>(i) * DrawMemorySpace * (1.0f + m_MemoryExpandRate) + MemoryOffset;

			if (IsLongMemory[LoopCounter])
			{
				// 長い針とメモリテキストを描画
				drawList->AddLine(ImVec2(x, cursorPos.y), ImVec2(x, cursorPos.y + 20.0f), IM_COL32(255, 255, 255, 255));

				std::string label = math::CMath::GetFloatWithPrecision(LargeMemoryValue, 3);
				drawList->AddText(ImVec2(x, cursorPos.y + 22.0f), IM_COL32(255, 255, 255, 255), label.c_str());

				// 長いメモリの値を更新
				LargeMemoryValue += m_LargeMemoryWidth;
			}
			else
			{
				// 短いメモリのみ
				drawList->AddLine(ImVec2(x, cursorPos.y), ImVec2(x, cursorPos.y + 10.0f), IM_COL32(255, 255, 255, 255));
			}
			
		}

		// インジケーターの描画
		if (!DrawIndicator(drawList, cursorPos, availableSize, barSize)) return false;

		return true;
	}

	bool CTimeLineView::CalcIndicator(const ImVec2& cursorPos, const ImVec2& barSize)
	{
		float DrawPos = cursorPos.x + m_IndicatorRate * barSize.x;

		float btnW = 10.0f;
		ImGui::SetCursorScreenPos(ImVec2(DrawPos - btnW * 0.5f, cursorPos.y));
		
		bool IsClicked = (ImGui::IsMouseDown(0));

		if (!m_ClickedIndicator)
		{
			ImGui::InvisibleButton("##TimelineIndicator", ImVec2(btnW, barSize.y));
			if (ImGui::IsItemHovered() && IsClicked)
			{
				m_ClickedIndicator = true;
			}
		}

		if (m_ClickedIndicator && IsClicked)
		{
			ImVec2 mousePos = ImGui::GetMousePos();

			m_IndicatorRate = (mousePos.x - cursorPos.x) / barSize.x;
			m_IndicatorRate = glm::clamp(m_IndicatorRate, 0.0f, 1.0f);
		}
		else
		{
			m_ClickedIndicator = false;
		}

		return true;
	}

	bool CTimeLineView::DrawIndicator(ImDrawList* drawList, const ImVec2& cursorPos, const ImVec2& availableSize, const ImVec2& barSize)
	{
		float DrawPos = cursorPos.x + m_IndicatorRate * barSize.x;
		drawList->AddLine(ImVec2(DrawPos, cursorPos.y), ImVec2(DrawPos, cursorPos.y + availableSize.y), IM_COL32(255, 0, 0, 255));

		return true;
	}

	bool CTimeLineView::CheckWheelExpand()
	{
		// インジケーターと一緒に動かないようにする
		if (m_ClickedIndicator) return true;

		// マウスホイール量で拡大率を更新
		ImGuiIO& io = ImGui::GetIO();
		const float MouseWheel = io.MouseWheel;

		if (ImGui::IsWindowHovered() && MouseWheel != 0.0f)
		{
			if (glm::sign(MouseWheel) == 1.0f && m_LargeMemoryWidth >= 100.0f)
			{
				// 最大値は100.0
				return true;
			}
			else if (glm::sign(MouseWheel) == -1.0f && m_LargeMemoryWidth <= 0.01f)
			{
				// 最小値は0.01
				return true;
			}

			const float Speed = 0.05f;
			const float Width = 0.1f;

			m_MemoryExpandRate += MouseWheel * Speed;

			if (m_MemoryExpandRate >= Width)
			{
				// 長いメモリの値を大きくする
				m_LargeMemoryWidth *= 10.0f; 
				m_MemoryExpandRate = 0.0f;
			}
			else if (m_MemoryExpandRate <= -Width)
			{
				// 長いメモリの値を小さくする
				m_LargeMemoryWidth *= 0.1f; 
				m_MemoryExpandRate = 0.0f;
			}
		}

		return true;
	}

	bool CTimeLineView::CheckMemoryDrag(const ImVec2& availableSize, float DrawMemorySpace, float MaxTime)
	{
		// インジケーターと一緒に動かないようにする
		if (m_ClickedIndicator) return true;

		const bool IsMemoryHovered = (ImGui::IsItemHovered() && ImGui::IsMouseDragging(0));
		const bool IsTLMiddleDrag = (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(2));

		// マウスドラッグでメモリの左端と右端の値を更新
		if (IsMemoryHovered || IsTLMiddleDrag)
		{
			ImVec2 MousePos = ImGui::GetMousePos();

			if (!m_FirstClicked)
			{
				// マウスの移動量。描画可能範囲で正規化する
				float OffsetX = (MousePos.x - m_PrevMousePos.x) / availableSize.x; // 左端から右端に行けたら１が返る
				// 例. メモリの長針のサイズが1.0秒として2.0秒からから3.0秒に移動したときに1.0動くようにする
				//OffsetX = OffsetX * DrawMemorySpace * 4.0f;
				//OffsetX = OffsetX * DrawMemorySpace;
				OffsetX = OffsetX * DrawMemorySpace / 4.0f;

				// 長針サイズで拡大縮小する
				OffsetX *= m_LargeMemoryWidth;

				m_LeftSideMemory -= OffsetX;
				m_RightSideMemory -= OffsetX;

				if (m_LeftSideMemory < 0.0f)
				{
					// 0よりも左に行かないようにする
					m_LeftSideMemory = 0.0f;
					m_RightSideMemory = static_cast<float>(m_MaxLargeMemoryCount) * m_LargeMemoryWidth;
				}
			}

			m_PrevMousePos = MousePos;
			m_FirstClicked = false;
		}
		else
		{
			m_FirstClicked = true;
		}

		return true;
	}

	float CTimeLineView::GetFirstLargeMemory(float SrcValue, std::vector<bool>& IsLongMemory)
	{
		// 一番初めに出てくる長いメモリの値を取得
		float DecimalPoint = SrcValue - floorf(SrcValue);
		
		float DstValue = 0.0f;

		if (DecimalPoint == 0.0f)
		{
			DstValue = SrcValue;
			IsLongMemory = std::vector<bool>({ true, false, false });
		}
		else if (DecimalPoint > 0.0f && DecimalPoint <= 0.3f)
		{
			DstValue = floorf(SrcValue) + 1.0f;
			IsLongMemory = std::vector<bool>({ false, false, true });
		}
		else if (DecimalPoint > 0.3f && DecimalPoint <= 6.0f)
		{
			DstValue = floorf(SrcValue) + 1.0f;
			IsLongMemory = std::vector<bool>({ false, true, false });
		}
		else if (DecimalPoint > 0.6f && DecimalPoint < 1.0f)
		{
			DstValue = floorf(SrcValue) + 1.0f;
			IsLongMemory = std::vector<bool>({ true, false, false });
		}

		return DstValue;
	}

	float CTimeLineView::GetFirstMemory(float SrcValue)
	{
		// 一番初めに出てくるメモリの値を取得(長短関係なし)
		float DecimalPoint = SrcValue - floorf(SrcValue);

		float DstValue = 0.0f;

		if (DecimalPoint == 0.0f)
		{
			DstValue = SrcValue;
		}
		else if (DecimalPoint > 0.0f && DecimalPoint <= 0.3f)
		{
			DstValue = floorf(SrcValue) + 0.3f;
		}
		else if (DecimalPoint > 0.3f && DecimalPoint <= 6.0f)
		{
			DstValue = floorf(SrcValue) + 0.6f;
		}
		else if (DecimalPoint > 0.6f && DecimalPoint < 1.0f)
		{
			DstValue = floorf(SrcValue) + 1.0f;
		}

		return DstValue;
	}

	void CTimeLineView::TestMemoryBar(const std::shared_ptr<timeline::CTimelineController>& TimelineController)
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
		drawList->AddLine(ImVec2(CurrentFrameX, cursorPos.y), ImVec2(CurrentFrameX, cursorPos.y + availableSize.y), IM_COL32(255, 0, 0, 255));

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
	}
}
#endif