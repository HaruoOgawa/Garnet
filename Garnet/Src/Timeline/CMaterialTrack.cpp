#include "CMaterialTrack.h"

namespace timeline
{
	CMaterialTrack::CMaterialTrack(int SamplerIndex, animation::EAnimationTarget AnimationTarget, const STimelineConnector& Connector):
		CTimelineTrack(SamplerIndex, AnimationTarget, Connector)
	{
	}

	CMaterialTrack::~CMaterialTrack()
	{
	}

	bool CMaterialTrack::Update(float CurrentTime, const std::vector<float>& Value, const std::vector<object::C3DObject>& ObjectList)
	{
		return true;
	}
}