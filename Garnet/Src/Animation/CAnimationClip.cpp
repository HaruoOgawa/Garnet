#include "CAnimationClip.h"

namespace animation
{
	CAnimationClip::CAnimationClip():
		m_CurrentTime(0.0f),
		m_DefaultSkin(nullptr)
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

	void CAnimationClip::SetDefaultSkin(const std::shared_ptr<animation::CSkin>& DefaultSkin)
	{
		m_DefaultSkin = DefaultSkin;
	}

	const std::shared_ptr<animation::CSkin>& CAnimationClip::GetDefaultSkin() const
	{
		return m_DefaultSkin;
	}

	const std::vector<std::shared_ptr<animation::CAnimationSampler>>& CAnimationClip::GetSamplerList() const
	{
		return m_SamplerList;
	}

	const std::vector<std::shared_ptr<animation::CAnimationChannel>>& CAnimationClip::GetChannelList() const
	{
		return m_ChannelList;
	}

	bool CAnimationClip::Update(float DeltaSecondsTime)
	{
		m_CurrentTime += DeltaSecondsTime;

		if (m_SamplerList.size() > 0)
		{
			m_CurrentTime = glm::mod(m_CurrentTime, m_SamplerList[0]->GetEndTime());
		}

		if (!UpdateFrame(m_CurrentTime)) return false;

		return true;
	}

	bool CAnimationClip::UpdateFrame(float CurrentTime)
	{
		for (const auto& Channel : m_ChannelList)
		{
			int SamplerIndex = Channel->GetSamplerIndex();
			if (SamplerIndex < 0 || SamplerIndex >= m_SamplerList.size()) continue;

			const auto& Sampler = m_SamplerList[SamplerIndex];
			std::vector<float> Value;

			if (!Sampler->ComputeCurrentFrame(CurrentTime, Value, Channel->GetAnimationTarget())) return false;

			if (!Channel->Update(Value)) return false;
		}

		return true;
	}

	int CAnimationClip::GetFrameCount()
	{
		int Count = 0;

		if (m_SamplerList.size() > 0)
		{
			Count = static_cast<int>(m_SamplerList[0]->GetKeyFrameList().size());
		}

		return Count;
	}

	// ˆê”ÔƒtƒŒ[ƒ€”‚ª‘½‚¢Sampler‚ğæ“¾‚·‚é
	std::shared_ptr<animation::CAnimationSampler> CAnimationClip::GetSamplerWithMostFrames()
	{
		std::shared_ptr<animation::CAnimationSampler> Result = nullptr;

		size_t maxSize = 0;

		for (const auto& Sampler : m_SamplerList)
		{
			size_t size = Sampler->GetKeyFrameList().size();

			if (size > maxSize)
			{
				size = maxSize;

				Result = Sampler;
			}
		}

		return Result;
	}

	float CAnimationClip::GetCurrentTime() const
	{
		return m_CurrentTime;
	}
}