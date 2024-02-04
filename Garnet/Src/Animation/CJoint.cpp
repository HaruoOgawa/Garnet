#ifdef USE_ANIMATION

#include "CJoint.h"

namespace animation
{
	CJoint::CJoint(const std::shared_ptr<object::CNode>& JointNode):
		m_JointNode(JointNode),
		m_BoneName(EHumanoidBones::None),
		m_ParentBoneName(EHumanoidBones::None),
		m_GrantParentBoneIndex(-1),
		m_GrantRate(0.0f),
		m_RotateGrant(false),
		m_MoveGrant(false),
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

	// ïtó^êeÉ{Å[ÉìÇÃÉ{Å[ÉìIndex
	int CJoint::GetGrantParentBoneIndex() const
	{
		return m_GrantParentBoneIndex;
	}

	// ïtó^ó¶
	float CJoint::GetGrantRate() const
	{
		return m_GrantRate;
	}

	// âÒì]ïtó^
	void CJoint::SetRotateGrant(int GrantParentBoneIndex, float GrantRate)
	{
		m_RotateGrant = true;

		m_GrantParentBoneIndex = GrantParentBoneIndex;
		m_GrantRate = GrantRate;
	}

	bool CJoint::IsRotateGrant() const
	{
		return m_RotateGrant;
	}

	// à⁄ìÆïtó^
	void CJoint::SetMoveGrant(int GrantParentBoneIndex, float GrantRate)
	{
		m_MoveGrant = true;

		m_GrantParentBoneIndex = GrantParentBoneIndex;
		m_GrantRate = GrantRate;
	}

	bool CJoint::IsMoveGrant() const
	{
		return m_MoveGrant;
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