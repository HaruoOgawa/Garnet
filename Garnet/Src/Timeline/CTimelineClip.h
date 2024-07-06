#pragma once

#include <memory>
#include <vector>

#include "../Animation/CAnimationSampler.h"
#include "CTimelineTrack.h"

namespace object { class C3DObject; }

namespace timeline
{
	class CTimelineClip
	{
		std::vector<std::shared_ptr<animation::CAnimationSampler>> m_SamplerList;
		std::vector<std::shared_ptr<CTimelineTrack>> m_TrackList;
	public:
		CTimelineClip();
		virtual ~CTimelineClip();

		bool Update(float CurrentTime, const std::vector<object::C3DObject>& ObjectList);

		void AddSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler);
		void AddTrack(const std::shared_ptr<CTimelineTrack>& Track);
	};
}