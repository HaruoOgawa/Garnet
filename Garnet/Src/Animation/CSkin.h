#pragma once

// 用語を整理すると、Jointは関節(点)でjointとJointの繋がりがBone(線)
// Boneを組み合わせた全体構造がSkeleton もしくは Skin
// しかしglTFのSkeletonプロパティはアニメーションのルートを示すもので、なくてもルートはわかるものなので

#include "CJoint.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace animation
{
	class CSkin
	{
		std::vector<std::shared_ptr<CJoint>> m_JointList;
		std::vector<glm::mat4> m_InverseBindMatrixList;
	public:
		CSkin();
		virtual ~CSkin();

		void AddJoint(const std::shared_ptr<CJoint>& Joint);
		void AddInverseBindMatrices(const std::vector<glm::mat4>& Matrices);

		bool CalcSkinMatrixList(std::vector<glm::mat4>& MatrixList);

		const std::vector<std::shared_ptr<CJoint>>& GetJointList() const;
	};
}