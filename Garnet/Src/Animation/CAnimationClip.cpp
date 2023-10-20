#include "CAnimationClip.h"

namespace animation
{
	CAnimationClip::CAnimationClip()
	{
	}

	CAnimationClip::~CAnimationClip()
	{
	}

	void CAnimationClip::AnimationSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler)
	{
		m_SamplerList.push_back(Sampler);
	}
}