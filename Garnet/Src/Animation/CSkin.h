#pragma once
#ifdef USE_ANIMATION

// 用語を整理すると、Jointは関節(点)でjointとJointの繋がりがBone(線)
// Boneを組み合わせた全体構造がSkeleton もしくは Skin
// しかしglTFのSkeletonプロパティはアニメーションのルートを示すもので、なくてもルートはわかるものなので

#include "CJoint.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

namespace animation
{
	class CSkin
	{
		std::vector<std::shared_ptr<CJoint>> m_JointList;
		int m_JointIndexOffset;

		std::unordered_map<EHumanoidBones, std::shared_ptr<CJoint>> m_BoneTable;

		// IK
		std::vector<std::shared_ptr<CJoint>> m_IKBoneList;

		// 付与ボーンリスト
		std::vector<std::shared_ptr<CJoint>> m_GrantBoneList;
	public:
		CSkin();
		virtual ~CSkin();

		void AddJoint(const std::shared_ptr<CJoint>& Joint);
		
		bool CalcSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix);

		const std::vector<std::shared_ptr<CJoint>>& GetJointList() const;

		void SetJointIndexOffset(int JointIndexOffset);
		int GetJointIndexOffset() const;

		void MakeBoneTable();
		const std::unordered_map<EHumanoidBones, std::shared_ptr<CJoint>>& GetBoneTable() const;
		std::shared_ptr<CJoint> GetBone(EHumanoidBones BoneName);

		// IK
		void MakeIKBoneList();
		const std::vector<std::shared_ptr<CJoint>>& GetIKBoneList() const;

		// 付与ボーン
		void MakeGrantBoneList();
		const std::vector<std::shared_ptr<CJoint>>& GetGrantBoneList() const;

		//
		void CalcSkinWorldMatrix();
		void ResetToDefaultSkinLocal();
	};
}

#endif // USE_ANIMATION