#pragma once
#ifdef USE_ANIMATION

// 用語を整理すると、Boneは関節(点)でBoneとBoneの繋がりがBone(線)
// Boneを組み合わせた全体構造がSkeleton もしくは Skin
// しかしglTFのSkeletonプロパティはアニメーションのルートを示すもので、なくてもルートはわかるものなのであまり意味をなしていない

#include <memory>
#include "EHumanoidBones.h"
#include "SIKParam.h"

namespace object { class CNode; }

namespace animation
{
	class CBone
	{
		std::shared_ptr<object::CNode> m_BoneNode;

		EHumanoidBones m_BoneName;

		EHumanoidBones m_ParentBoneName;

		// 回転付与・移動付与
		// 付与とは他のボーンに付いて行くということ
		// 付与親ボーンのボーンIndex
		int m_GrantParentBoneIndex;

		// 付与率
		float m_GrantRate;

		// 回転付与
		bool m_RotateGrant;

		// 移動付与
		bool m_MoveGrant;

		// IK
		std::shared_ptr<animation::SIKParam> m_IKParam;
	public:
		CBone(const std::shared_ptr<object::CNode>& BoneNode);
		virtual ~CBone();

		const std::shared_ptr<object::CNode>& GetBoneNode() const;

		EHumanoidBones GetBoneName() const;
		void SetBoneName(EHumanoidBones BoneName);

		EHumanoidBones GetParentBoneName() const;
		void SetParentBoneName(EHumanoidBones BoneName);

		// 付与親ボーンのボーンIndex
		int GetGrantParentBoneIndex() const;

		// 付与率
		float GetGrantRate() const;

		// 回転付与
		void SetRotateGrant(int GrantParentBoneIndex, float GrantRate);
		bool IsRotateGrant() const;

		// 移動付与
		void SetMoveGrant(int GrantParentBoneIndex, float GrantRate);
		bool IsMoveGrant() const;

		// IK
		const std::shared_ptr<animation::SIKParam>& GetIKParam() const;
		void SetIKParam(const std::shared_ptr<animation::SIKParam>& Param);
	};
}

#endif // USE_ANIMATION