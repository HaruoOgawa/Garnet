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
	public:
		CAnimationClip();
		virtual ~CAnimationClip();

		void AnimationSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler);
	};
}