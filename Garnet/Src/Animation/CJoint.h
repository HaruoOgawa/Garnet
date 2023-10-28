#pragma once

// VRM用の拡張としてskeletonフィールドがあるようなのでひとまずSkeleton - BoneではなくglTFの仕様に沿って名称は Skin - Joint のペアにした

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