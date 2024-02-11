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

		// Translate成分はオフセットなのか座標なのか
		// MMDはオフセットとして扱う
		const bool m_IsTransOffset;

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
		CAnimationChannel(bool UseAnimLocalAxis, bool TransOffset, int SamplerIndex, EAnimationTarget AnimationTarget, const std::shared_ptr<object::CNode>& TargetNode, EHumanoidBones BoneName);
		virtual ~CAnimationChannel();

		bool IsUseAnimLocalAxis() const;
		bool IsTransOffset() const;

		int GetSamplerIndex() const;
		EAnimationTarget GetAnimationTarget() const;

		EHumanoidBones GetBoneName() const;

		bool Update(const std::vector<float>& Value);
	};
}

#endif // USE_ANIMATION