#pragma once

#include <string>
#include <memory>
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include "EAnimationTarget.h"
#include "EHumanoidBones.h"

namespace object { class CNode; }

namespace animation
{
	class CAnimationChannel
	{
		const int m_SamplerIndex;
		const EAnimationTarget m_AnimationTarget;
		std::shared_ptr<object::CNode> m_TargetNode;
		EHumanoidBones m_BoneName;
	private:
		bool UpdateTranslation(const std::vector<float>& Value);
		bool UpdateRotation(const std::vector<float>& Value);
		bool UpdateScale(const std::vector<float>& Value);
		bool UpdateWeights(const std::vector<float>& Value);
		bool UpdateModelMatrix(const std::vector<float>& Value);
	public:
		CAnimationChannel(int SamplerIndex, EAnimationTarget AnimationTarget, const std::shared_ptr<object::CNode>& TargetNode, EHumanoidBones BoneName);
		virtual ~CAnimationChannel();

		int GetSamplerIndex() const;
		EAnimationTarget GetAnimationTarget() const;

		void SetTargetNode(const std::shared_ptr<object::CNode>& TargetNode);

		EHumanoidBones GetBoneName() const;

		bool Update(const std::vector<float>& Value);
	};
}