#pragma once

#include <vector>
#

#include "CAnimationSampler.h"
#include "CAnimationChannel.h"

namespace animation
{
	class CAnimationClip
	{
		std::vector<std::shared_ptr<animation::CAnimationSampler>> m_SamplerList;
		std::vector<std::shared_ptr<animation::CAnimationChannel>> m_AnimationChannelList;
	public:
		CAnimationClip();
		virtual ~CAnimationClip();

		void AddAnimationSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler);
		void AddAnimationChannel(const std::shared_ptr<animation::CAnimationChannel>& AnimationChannel);
	};
}