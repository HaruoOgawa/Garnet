#pragma once

#include <string>
#include <memory>

namespace object { class CNode; }

namespace animation
{
	enum class EAnimationTarget
	{
		NONE = -1,

		TRANSLATION,
		ROTATION,
		SCALE,
		WEIGHTS,
	};

	class CAnimationChannel
	{
		const int m_SamplerIndex;
		const EAnimationTarget m_AnimationTarget;
		std::shared_ptr<object::CNode> m_TargetNode;
	public:
		CAnimationChannel(int SamplerIndex, EAnimationTarget AnimationTarget, const std::shared_ptr<object::CNode>& TargetNode);
		virtual ~CAnimationChannel();
	};
}