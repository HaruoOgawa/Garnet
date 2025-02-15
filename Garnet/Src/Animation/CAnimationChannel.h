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

		std::string m_TargetNodeName;
		EHumanoidBones m_BoneName;

		// アニメーションの元の持ち主が指していたノード
		// アニメーションを他のモデルに渡すことがない非ボーンの階層アニメーションでこれを使う
		std::shared_ptr<object::CNode> m_DefaultTargetNode;
	private:
		bool UpdateTranslation(const std::vector<float>& Value, const std::shared_ptr<object::CNode>& TargetNode);
		bool UpdateRotation(const std::vector<float>& Value, const std::shared_ptr<object::CNode>& TargetNode);
		bool UpdateScale(const std::vector<float>& Value, const std::shared_ptr<object::CNode>& TargetNode);
		bool UpdateWeights(const std::vector<float>& Value, const std::shared_ptr<object::CNode>& TargetNode);
		bool UpdateModelMatrix(const std::vector<float>& Value, const std::shared_ptr<object::CNode>& TargetNode);
	public:
		CAnimationChannel(bool UseAnimLocalAxis, bool TransOffset, int SamplerIndex, EAnimationTarget AnimationTarget, 
			const std::string& TargetNodeName, EHumanoidBones BoneName, const std::shared_ptr<object::CNode>& DefaultTargetNode);
		virtual ~CAnimationChannel();

		bool IsUseAnimLocalAxis() const;
		bool IsTransOffset() const;

		int GetSamplerIndex() const;
		EAnimationTarget GetAnimationTarget() const;

		EHumanoidBones GetBoneName() const;

		std::string GetTargetNodeName() const;

		const std::shared_ptr<object::CNode>& GetDefaultTargetNode() const;

		bool Update(const std::vector<float>& Value, const std::shared_ptr<object::CNode>& TargetNode);
	};
}

#endif // USE_ANIMATION