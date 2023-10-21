#include "CAnimationChannel.h"
#include "../Object/CNode.h"

namespace animation
{
	CAnimationChannel::CAnimationChannel(int SamplerIndex, EAnimationTarget AnimationTarget, const std::shared_ptr<object::CNode>& TargetNode):
		m_SamplerIndex(SamplerIndex),
		m_AnimationTarget(AnimationTarget),
		m_TargetNode(TargetNode)
	{
	}

	CAnimationChannel::~CAnimationChannel()
	{
	}
}