#pragma once
#ifdef USE_ANIMATION

#include <vector>

#include "CAnimationSampler.h"
#include "CAnimationChannel.h"
#include "EAnimationTarget.h"
#include "CSkeleton.h"

namespace animation
{
	class CAnimationClip
	{
		float m_CurrentTime;
		bool m_IsLoop;

		std::vector<std::shared_ptr<animation::CAnimationSampler>> m_SamplerList;
		std::vector<std::shared_ptr<animation::CAnimationChannel>> m_ChannelList;

		std::shared_ptr<animation::CSkeleton> m_DefaultSkeleton;
	public:
		CAnimationClip();
		virtual ~CAnimationClip();

		void SetIsLoop(bool val);
		bool IsLoop() const;

		void Initialize();

		void AddAnimationSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler);
		void AddAnimationChannel(const std::shared_ptr<animation::CAnimationChannel>& AnimationChannel);

		void SetDefaultSkeleton(const std::shared_ptr<animation::CSkeleton>& DefaultSkeleton);
		const std::shared_ptr<animation::CSkeleton>& GetDefaultSkeleton() const;

		const std::vector<std::shared_ptr<animation::CAnimationSampler>>& GetSamplerList() const;
		const std::vector<std::shared_ptr<animation::CAnimationChannel>>& GetChannelList() const;

		bool Update(float DeltaSecondsTime);
		bool UpdateFrame(float CurrentTime);

		int GetFrameCount();

		// ˆê”ÔƒtƒŒ[ƒ€”‚ª‘½‚¢Sampler‚ğæ“¾‚·‚é
		std::shared_ptr<animation::CAnimationSampler> GetSamplerWithMostFrames();

		float GetCurrentTime() const;
		bool IsEnd();
	};
}

#endif // USE_ANIMATION