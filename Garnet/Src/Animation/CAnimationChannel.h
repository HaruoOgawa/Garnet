#pragma once

#include <string>
#include <memory>
#include <vector>
#include <glm/gtc/quaternion.hpp>

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
	private:
		bool UpdateTranslation(const std::vector<float>& Value);
		bool UpdateRotation(const std::vector<float>& Value);
		bool UpdateScale(const std::vector<float>& Value);
		bool UpdateWeights(const std::vector<float>& Value);
	public:
		CAnimationChannel(int SamplerIndex, EAnimationTarget AnimationTarget, const std::shared_ptr<object::CNode>& TargetNode);
		virtual ~CAnimationChannel();

		int GetSamplerIndex() const;
		EAnimationTarget GetAnimationTarget() const;

		bool Update(const std::vector<float>& Value);
	};
}