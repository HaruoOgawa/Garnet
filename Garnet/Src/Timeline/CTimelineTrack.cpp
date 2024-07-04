#include "CTimelineTrack.h"

namespace timeline
{
	CTimelineTrack::CTimelineTrack():
		m_SamplerIndex(-1),
		m_TrackType(ETimelineTrackType::None),
		m_Connector({})
	{
	}

	int CTimelineTrack::GetSamplerIndex() const
	{
		return m_SamplerIndex;
	}
}