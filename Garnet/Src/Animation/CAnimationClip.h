#pragma once
#ifdef USE_ANIMATION

#include <vector>

#include "CAnimationSampler.h"
#include "CAnimationChannel.h"
#include "EAnimationTarget.h"
#include "CSkeleton.h"

namespace animation
{
	class CSkeleton;

	class CAnimationClip
	{
		std::shared_ptr<animation::CSkeleton> m_DefaultSkeleton;

	protected:
		float m_CurrentTime;
		float m_ClipStartTime;
		float m_ClipEndTime;

		bool m_IsLoop;

		bool m_UseIK;

		std::vector<std::shared_ptr<animation::CAnimationSampler>> m_SamplerList;
		std::vector<std::shared_ptr<animation::CAnimationChannel>> m_ChannelList;
		
	private:
		std::vector<float> GetDefaultValueFromAnimationTarget(EAnimationTarget AnimationTarget);

		std::shared_ptr<object::CNode> FindTargetNode(const std::shared_ptr<CSkeleton>& Skeleton, const std::shared_ptr<animation::CAnimationChannel>& Channel);

	public:
		CAnimationClip();
		virtual ~CAnimationClip();

		virtual void SetIsLoop(bool val);
		virtual bool IsLoop() const;

		virtual void SetUseIK(bool Val);
		virtual bool IsUseIK() const;

		virtual void Reset(const std::shared_ptr<CSkeleton>& Skeleton);

		virtual void AddAnimationSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler);
		virtual void AddAnimationChannel(const std::shared_ptr<animation::CAnimationChannel>& AnimationChannel);

		virtual void SetDefaultSkeleton(const std::shared_ptr<animation::CSkeleton>& DefaultSkeleton);
		virtual const std::shared_ptr<animation::CSkeleton>& GetDefaultSkeleton() const;

		virtual const std::vector<std::shared_ptr<animation::CAnimationSampler>>& GetSamplerList() const;
		virtual const std::vector<std::shared_ptr<animation::CAnimationChannel>>& GetChannelList() const;

		virtual bool Update(float DeltaSecondsTime, const std::shared_ptr<CSkeleton>& Skeleton);
		virtual bool UpdateFrame(float CurrentTime, const std::shared_ptr<CSkeleton>& Skeleton);

		virtual int GetFrameCount();

		// àÍî‘ÉtÉåÅ[ÉÄêîÇ™ëΩÇ¢SamplerÇéÊìæÇ∑ÇÈ
		virtual std::shared_ptr<animation::CAnimationSampler> GetSamplerWithMostFrames();

		virtual void SetCurrentTime(float Time);
		virtual float GetCurrentTime() const;
		virtual float GetEndTime() const;

		virtual bool IsEnd();
	};
}

#endif // USE_ANIMATION