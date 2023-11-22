#pragma once

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
	public:
		CSkin();
		virtual ~CSkin();

		void AddJoint(const std::shared_ptr<CJoint>& Joint);
		
		bool CalcSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix);

		const std::vector<std::shared_ptr<CJoint>>& GetJointList() const;

		void SetJointIndexOffset(int JointIndexOffset);
		int GetJointIndexOffset() const;

		void MakeBoneTable();
		std::shared_ptr<CJoint> GetBone(EHumanoidBones BoneName);
	};
}