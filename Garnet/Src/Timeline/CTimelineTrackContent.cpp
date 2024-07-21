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

	void CTimelineTrackContent::RemoveRefTrackID(const std::string& TrackID)
	{
		const auto it = std::find(m_RefTrackIDList.begin(), m_RefTrackIDList.end(), TrackID);

		if (it != m_RefTrackIDList.end())
		{
			m_RefTrackIDList.erase(it);
			m_RefTrackIDList.shrink_to_fit();
		}
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