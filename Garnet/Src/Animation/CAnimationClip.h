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
		std::shared_ptr<animation::CSkeleton> m_DefaultSkeleton;

	protected:
		float m_CurrentTime;

		bool m_IsLoop;

		bool m_UseIK;

		std::vector<std::shared_ptr<animation::CAnimationSampler>> m_SamplerList;
		std::vector<std::shared_ptr<animation::CAnimationChannel>> m_ChannelList;
		
	public:
		CAnimationClip();
		virtual ~CAnimationClip();

		virtual void SetIsLoop(bool val);
		virtual bool IsLoop() const;

		virtual void SetUseIK(bool Val);
		virtual bool IsUseIK() const;

		virtual void Initialize();

		virtual void AddAnimationSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler);
		virtual void AddAnimationChannel(const std::shared_ptr<animation::CAnimationChannel>& AnimationChannel);

		virtual void SetDefaultSkeleton(const std::shared_ptr<animation::CSkeleton>& DefaultSkeleton);
		virtual const std::shared_ptr<animation::CSkeleton>& GetDefaultSkeleton() const;

		virtual const std::vector<std::shared_ptr<animation::CAnimationSampler>>& GetSamplerList() const;
		virtual const std::vector<std::shared_ptr<animation::CAnimationChannel>>& GetChannelList() const;

		virtual bool Update(float DeltaSecondsTime);
		virtual bool UpdateFrame(float CurrentTime);

		virtual int GetFrameCount();

		// àÍî‘ÉtÉåÅ[ÉÄêîÇ™ëΩÇ¢SamplerÇéÊìæÇ∑ÇÈ
		virtual std::shared_ptr<animation::CAnimationSampler> GetSamplerWithMostFrames();

		virtual float GetCurrentTime() const;
		virtual bool IsEnd();
	};
}

#endif // USE_ANIMATION