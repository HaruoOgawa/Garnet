#pragma once
#ifdef USE_ANIMATION

// 用語を整理すると、Jointは関節(点)でjointとJointの繋がりがBone(線)
// Boneを組み合わせた全体構造がSkeleton もしくは Skin
// しかしglTFのSkeletonプロパティはアニメーションのルートを示すもので、なくてもルートはわかるものなのであまり意味をなしていない

#include <memory>
#include "EHumanoidBones.h"
#include "SIKParam.h"

namespace object { class CNode; }

namespace animation
{
	class CJoint
	{
		std::shared_ptr<object::CNode> m_JointNode;

		EHumanoidBones m_BoneName;

		EHumanoidBones m_ParentBoneName;

		// IK
		std::shared_ptr<animation::SIKParam> m_IKParam;
	public:
		CJoint(const std::shared_ptr<object::CNode>& JointNode);
		virtual ~CJoint();

		const std::shared_ptr<object::CNode>& GetJointNode() const;

		EHumanoidBones GetBoneName() const;
		void SetBoneName(EHumanoidBones BoneName);

		EHumanoidBones GetParentBoneName() const;
		void SetParentBoneName(EHumanoidBones BoneName);

		// IK
		const std::shared_ptr<animation::SIKParam>& GetIKParam() const;
		void SetIKParam(const std::shared_ptr<animation::SIKParam>& Param);
	};
}

#endif // USE_ANIMATION