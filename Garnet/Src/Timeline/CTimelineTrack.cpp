#include "CTimelineTrack.h"

namespace timeline
{
	CTimelineTrack::CTimelineTrack(int SamplerIndex, animation::EAnimationTarget AnimationTarget, const STimelineConnector& Connector):
		m_SamplerIndex(SamplerIndex),
		m_AnimationTarget(AnimationTarget),
		m_Connector(Connector)
	{
	}

	int CTimelineTrack::GetSamplerIndex() const
	{
		return m_SamplerIndex;
	}

	animation::EAnimationTarget CTimelineTrack::GetAnimationTarget() const
	{
		return m_AnimationTarget;
	}

	bool CTimelineTrack::Update(float CurrentTime, const std::vector<float>& Value, const std::vector<object::C3DObject>& ObjectList)
	{
		return true;
	}
}