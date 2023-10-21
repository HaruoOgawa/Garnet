#include "CAnimationClip.h"

namespace animation
{
	CAnimationClip::CAnimationClip():
		m_CurrentTime(0.0f)
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
		m_ChannelList.push_back(AnimationChannel);
	}

	bool CAnimationClip::Update(float DeltaSecondsTime)
	{
		m_CurrentTime += DeltaSecondsTime;

		for (const auto& Channel : m_ChannelList)
		{
			int SamplerIndex = Channel->GetSamplerIndex();
			if (SamplerIndex < 0 || SamplerIndex >= m_SamplerList.size()) continue;

			const auto& Sampler = m_SamplerList[SamplerIndex];
			std::vector<float> Value;

			bool IsRot = (Channel->GetAnimationTarget() == EAnimationTarget::ROTATION);

			if (!Sampler->GetCurrentFrame(m_CurrentTime, Value, IsRot)) return false;

			if (!Channel->Update(Value)) return false;
		}

		return true;
	}
}