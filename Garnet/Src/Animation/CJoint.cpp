#ifdef USE_ANIMATION

#include "CJoint.h"

namespace animation
{
	CJoint::CJoint(const std::shared_ptr<object::CNode>& JointNode):
		m_JointNode(JointNode),
		m_BoneName(EHumanoidBones::None),
		m_ParentBoneName(EHumanoidBones::None),
		m_IKParam(nullptr)
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

	EHumanoidBones CJoint::GetParentBoneName() const
	{
		return m_ParentBoneName;
	}

	void CJoint::SetParentBoneName(EHumanoidBones BoneName)
	{
		m_ParentBoneName = BoneName;
	}

	// IK
	const std::shared_ptr<animation::SIKParam>& CJoint::GetIKParam() const
	{
		return m_IKParam;
	}

	void CJoint::SetIKParam(const std::shared_ptr<animation::SIKParam>& Param)
	{
		m_IKParam = Param;
	}
}

#endif // USE_ANIMATION