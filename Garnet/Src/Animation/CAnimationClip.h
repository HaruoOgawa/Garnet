#pragma once
#ifdef USE_ANIMATION

#include <vector>

#include "CAnimationSampler.h"
#include "CAnimationChannel.h"
#include "EAnimationTarget.h"
#include "CSkin.h"

namespace animation
{
	class CAnimationClip
	{
		float m_CurrentTime;

		std::vector<std::shared_ptr<animation::CAnimationSampler>> m_SamplerList;
		std::vector<std::shared_ptr<animation::CAnimationChannel>> m_ChannelList;

		std::shared_ptr<animation::CSkin> m_DefaultSkin;
	public:
		CAnimationClip();
		virtual ~CAnimationClip();

		void AddAnimationSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler);
		void AddAnimationChannel(const std::shared_ptr<animation::CAnimationChannel>& AnimationChannel);

		void SetDefaultSkin(const std::shared_ptr<animation::CSkin>& DefaultSkin);
		const std::shared_ptr<animation::CSkin>& GetDefaultSkin() const;

		const std::vector<std::shared_ptr<animation::CAnimationSampler>>& GetSamplerList() const;
		const std::vector<std::shared_ptr<animation::CAnimationChannel>>& GetChannelList() const;

		bool Update(float DeltaSecondsTime);
		bool UpdateFrame(float CurrentTime);

		int GetFrameCount();

		// ˆê”ÔƒtƒŒ[ƒ€”‚ª‘½‚¢Sampler‚ğæ“¾‚·‚é
		std::shared_ptr<animation::CAnimationSampler> GetSamplerWithMostFrames();

		float GetCurrentTime() const;
	};
}

#endif // USE_ANIMATION