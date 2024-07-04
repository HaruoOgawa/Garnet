#pragma once

#include "STimelineConnector.h"
#include "ETimelineTrackType.h"

namespace timeline
{
	class CTimelineTrack
	{
		int m_SamplerIndex;

		ETimelineTrackType m_TrackType;

		STimelineConnector m_Connector;
	public:
		CTimelineTrack();
		virtual ~CTimelineTrack() = default;

		int GetSamplerIndex() const;
	};
}