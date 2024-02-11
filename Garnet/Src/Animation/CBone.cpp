#ifdef USE_ANIMATION

#include "CBone.h"

namespace animation
{
	CBone::CBone(const std::shared_ptr<object::CNode>& BoneNode):
		m_BoneNode(BoneNode),
		m_BoneName(EHumanoidBones::None),
		m_ParentBoneName(EHumanoidBones::None),
		m_GrantParentBoneIndex(-1),
		m_GrantRate(0.0f),
		m_RotateGrant(false),
		m_MoveGrant(false),
		m_IKParam(nullptr)
	{
	}

	CBone::~CBone()
	{
	}

	const std::shared_ptr<object::CNode>& CBone::GetBoneNode() const
	{
		return m_BoneNode;
	}

	EHumanoidBones CBone::GetBoneName() const
	{
		return m_BoneName;
	}

	void CBone::SetBoneName(EHumanoidBones BoneName)
	{
		m_BoneName = BoneName;
	}

	EHumanoidBones CBone::GetParentBoneName() const
	{
		return m_ParentBoneName;
	}

	void CBone::SetParentBoneName(EHumanoidBones BoneName)
	{
		m_ParentBoneName = BoneName;
	}

	// ïtó^êeÉ{Å[ÉìÇÃÉ{Å[ÉìIndex
	int CBone::GetGrantParentBoneIndex() const
	{
		return m_GrantParentBoneIndex;
	}

	// ïtó^ó¶
	float CBone::GetGrantRate() const
	{
		return m_GrantRate;
	}

	// âÒì]ïtó^
	void CBone::SetRotateGrant(int GrantParentBoneIndex, float GrantRate)
	{
		m_RotateGrant = true;

		m_GrantParentBoneIndex = GrantParentBoneIndex;
		m_GrantRate = GrantRate;
	}

	bool CBone::IsRotateGrant() const
	{
		return m_RotateGrant;
	}

	// à⁄ìÆïtó^
	void CBone::SetMoveGrant(int GrantParentBoneIndex, float GrantRate)
	{
		m_MoveGrant = true;

		m_GrantParentBoneIndex = GrantParentBoneIndex;
		m_GrantRate = GrantRate;
	}

	bool CBone::IsMoveGrant() const
	{
		return m_MoveGrant;
	}

	// IK
	const std::shared_ptr<animation::SIKParam>& CBone::GetIKParam() const
	{
		return m_IKParam;
	}

	void CBone::SetIKParam(const std::shared_ptr<animation::SIKParam>& Param)
	{
		m_IKParam = Param;
	}
}

#endif // USE_ANIMATION