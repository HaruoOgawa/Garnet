#include "CAnimationClip.h"

namespace animation
{
	CAnimationClip::CAnimationClip()
	{
	}

	CAnimationClip::~CAnimationClip()
	{
	}

	void CAnimationClip::AddAnimationSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler)
	{
		m_SamplerList.push_back(Sampler);
	}

	void CAnimationClip::AddAnimationChannel(const std::shared_ptr<animation::CAnimationChannel>& AnimationChannel)
	{
		m_AnimationChannelList.push_back(AnimationChannel);
	}
}