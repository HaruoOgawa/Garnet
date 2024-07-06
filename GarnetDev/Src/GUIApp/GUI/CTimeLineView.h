#pragma once

#ifdef USE_GUIENGINE
#include <imgui.h>
#include <string>
#include <memory>

namespace timeline { class CTimelineController; }

namespace gui
{
	class CTimeLineView
	{
	private:
		bool DrawTimeBar(const std::shared_ptr<timeline::CTimelineController>& TimelineController);

	public:
		CTimeLineView();
		virtual ~CTimeLineView() = default;

		bool Draw(const std::shared_ptr<timeline::CTimelineController>& TimelineController);
	};
}
#endif