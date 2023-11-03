#pragma once

// 用語を整理すると、Jointは関節(点)でjointとJointの繋がりがBone(線)
// Boneを組み合わせた全体構造がSkeleton もしくは Skin
// しかしglTFのSkeletonプロパティはアニメーションのルートを示すもので、なくてもルートはわかるものなのであまり意味をなしていない

#include <memory>

namespace object { class CNode; }

namespace animation
{
	class CJoint
	{
		std::shared_ptr<object::CNode> m_JointNode;

	public:
		CJoint(const std::shared_ptr<object::CNode>& JointNode);
		virtual ~CJoint();

		const std::shared_ptr<object::CNode>& GetJointNode() const;
	};
}