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

	void CTimelineTrackContent::SetRefTrackIDList(const std::vector<std::string>& TrackIDList)
	{
		m_RefTrackIDList = TrackIDList;
	}

	const std::vector<std::string>& CTimelineTrackContent::GetRefTrackIDList() const
	{
		return m_RefTrackIDList;
	}
}