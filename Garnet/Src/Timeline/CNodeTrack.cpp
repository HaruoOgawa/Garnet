#include "CNodeTrack.h"

namespace timeline
{
	CNodeTrack::CNodeTrack(int SamplerIndex, animation::EAnimationTarget AnimationTarget, const STimelineConnector& Connector):
		CTimelineTrack(SamplerIndex, AnimationTarget, Connector)
	{
	}

	CNodeTrack::~CNodeTrack()
	{
	}

	bool CNodeTrack::Update(float CurrentTime, const std::vector<float>& Value, const std::vector<object::C3DObject>& ObjectList)
	{
		return true;
	}
}