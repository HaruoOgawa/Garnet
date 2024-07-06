#pragma once

#include <vector>

#include "STimelineConnector.h"
#include "../Animation/EAnimationTarget.h"

namespace object { class C3DObject; }

namespace timeline
{
	class CTimelineTrack
	{
		int m_SamplerIndex;

		const animation::EAnimationTarget m_AnimationTarget;

		STimelineConnector m_Connector;
	public:
		CTimelineTrack(int SamplerIndex, animation::EAnimationTarget AnimationTarget, const STimelineConnector& Connector);
		virtual ~CTimelineTrack() = default;

		int GetSamplerIndex() const;
		animation::EAnimationTarget GetAnimationTarget() const;

		virtual bool Update(float CurrentTime, const std::vector<float>& Value, const std::vector<object::C3DObject>& ObjectList);
	};
}