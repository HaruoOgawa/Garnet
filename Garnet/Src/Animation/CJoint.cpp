#include "CJoint.h"

namespace animation
{
	CJoint::CJoint(const std::shared_ptr<object::CNode>& JointNode):
		m_JointNode(JointNode),
		m_BoneName(EHumanoidBones::None)
	{
	}

	CJoint::~CJoint()
	{
	}

	const std::shared_ptr<object::CNode>& CJoint::GetJointNode() const
	{
		return m_JointNode;
	}

	EHumanoidBones CJoint::GetBoneName() const
	{
		return m_BoneName;
	}

	void CJoint::SetBoneName(EHumanoidBones BoneName)
	{
		m_BoneName = BoneName;
	}
}