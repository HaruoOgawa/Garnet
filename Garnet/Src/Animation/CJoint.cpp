#include "CJoint.h"

namespace animation
{
	CJoint::CJoint(const std::shared_ptr<object::CNode>& JointNode):
		m_JointNode(JointNode)
	{
	}

	CJoint::~CJoint()
	{
	}

	const std::shared_ptr<object::CNode>& CJoint::GetJointNode() const
	{
		return m_JointNode;
	}
}