#include "CTimelineTrackContent.h"

namespace timeline
{
	CTimelineTrackContent::CTimelineTrackContent()
	{
	}
	
	void CTimelineTrackContent::AddRefTrackID(const std::string& TrackID)
	{
		m_RefTrackIDList.push_back(TrackID);
	}

	const std::vector<std::string>& CTimelineTrackContent::GetRefTrackIDList() const
	{
		return m_RefTrackIDList;
	}
}