#pragma once

#include <vector>
#include <string>

namespace timeline
{
	class CTimelineTrackContent
	{
		std::vector<std::string> m_RefTrackIDList;
	public:
		CTimelineTrackContent();
		virtual ~CTimelineTrackContent() = default;

		void AddRefTrackID(const std::string& TrackID);
		const std::vector<std::string>& GetRefTrackIDList() const;
	};
}