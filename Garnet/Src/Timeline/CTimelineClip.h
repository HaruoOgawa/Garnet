#pragma once

#include <memory>
#include <vector>

#include "../Animation/CAnimationSampler.h"
#include "CTimelineTrack.h"

namespace timeline
{
	class CTimelineClip
	{
		std::vector<std::shared_ptr<animation::CAnimationSampler>> m_SamplerList;
		std::vector<std::shared_ptr<CTimelineTrack>> m_TrackList;
	public:
		CTimelineClip();
		virtual ~CTimelineClip();

		bool Update(float CurrentTime);
	};
}