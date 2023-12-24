#pragma once
#ifdef USE_ANIMATION

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
		// アニメーションのローカル軸を使用するか
		// FBXでは必須でglTF/VRMでは不要
		const bool m_UseAnimLocalAxis;

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
		CAnimationChannel(bool UseAnimLocalAxis, int SamplerIndex, EAnimationTarget AnimationTarget, const std::shared_ptr<object::CNode>& TargetNode, EHumanoidBones BoneName);
		virtual ~CAnimationChannel();

		bool IsUseAnimLocalAxis() const;

		int GetSamplerIndex() const;
		EAnimationTarget GetAnimationTarget() const;

		EHumanoidBones GetBoneName() const;

		bool Update(const std::vector<float>& Value);
	};
}

#endif // USE_ANIMATION