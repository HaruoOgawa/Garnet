#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "../Animation/CAnimationSampler.h"
#include "CTimelineTrack.h"

namespace object { class C3DObject; }

namespace timeline
{
	class CTimelineClip
	{
		float m_MaxTime;

		std::vector<std::shared_ptr<animation::CAnimationSampler>> m_SamplerList;
		std::unordered_map<std::string, std::shared_ptr<CTimelineTrack>> m_TrackList;
	public:
		CTimelineClip(float MaxTime);
		virtual ~CTimelineClip();

		void SetMaxTime(float Time);
		float GetMaxTime() const;

		bool Update(float CurrentTime);

		void AddSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler);
		void AddTrack(const std::shared_ptr<CTimelineTrack>& Track);

		void AssignObjectResourceToTrack(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList);
		//void AssignRendererSettingsToTrack();
	};
}