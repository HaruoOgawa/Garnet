#pragma once

#ifdef USE_GUIENGINE
#include <imgui.h>
#include <string>
#include <memory>
#include <vector>

namespace timeline { class CTimelineController; }

namespace gui
{
	class CTimeLineView
	{
		const int m_MaxLargeMemoryCount = 20; // 長いメモリの最大数。メモリの数は定数。長いメモリと長いメモリの間に短いメモリが2本あるので長短合わせて最大60本

		float m_LeftSideMemory; // タイムラインの左端
		float m_RightSideMemory; // タイムラインの右端

		float m_LargeMemoryWidth; // 現在の長いメモリの間隔. 0.01, 0.1, 1.0, 10.0, 100.0といった感じで変化する
		float m_MemoryExpandRate; // メモリの拡大率

		bool m_FirstClicked;
		ImVec2 m_PrevMousePos;

		bool m_ClickedIndicator;
		float m_IndicatorRate;
	private:
		bool DrawTimeBar(const std::shared_ptr<timeline::CTimelineController>& TimelineController);
		bool DrawHierarchyWindow(const std::shared_ptr<timeline::CTimelineController>& TimelineController);
		bool DrawKeyFrameWindow(const std::shared_ptr<timeline::CTimelineController>& TimelineController);

		bool DrawMemoryBar(const std::shared_ptr<timeline::CTimelineController>& TimelineController);
		bool CalcIndicator(const std::shared_ptr<timeline::CTimelineController>& TimelineController, const ImVec2& cursorPos, const ImVec2& barSize);
		bool DrawIndicator(ImDrawList* drawList, const ImVec2& cursorPos, const ImVec2& availableSize, const ImVec2& barSize);

		bool CheckWheelExpand();
		bool CheckMemoryDrag(const std::shared_ptr<timeline::CTimelineController>& TimelineController, const ImVec2& availableSize, float DrawMemorySpace, float MaxTime);

		bool UpdateCurrentTimeFromMemoryBar(const std::shared_ptr<timeline::CTimelineController>& TimelineController);
		bool UpdateMemoryFromTimeBar(const std::shared_ptr<timeline::CTimelineController>& TimelineController);

		float GetFirstLargeMemory(float SrcValue, std::vector<bool>& IsLongMemory);
		float GetFirstMemory(float SrcValue);
		
	public:
		CTimeLineView();
		virtual ~CTimeLineView() = default;

		bool Draw(const std::shared_ptr<timeline::CTimelineController>& TimelineController);
	};
}
#endif