#ifdef USE_ANIMATION

#include "CAnimationChannel.h"
#include "../Object/CNode.h"

namespace animation
{
	CAnimationChannel::CAnimationChannel(int SamplerIndex, EAnimationTarget AnimationTarget, const std::shared_ptr<object::CNode>& TargetNode, EHumanoidBones BoneName):
		m_SamplerIndex(SamplerIndex),
		m_AnimationTarget(AnimationTarget),
		m_TargetNode(TargetNode),
		m_BoneName(BoneName)
	{
	}

	CAnimationChannel::~CAnimationChannel()
	{
	}

	int CAnimationChannel::GetSamplerIndex() const
	{
		return m_SamplerIndex;
	}

	EAnimationTarget CAnimationChannel::GetAnimationTarget() const
	{
		return m_AnimationTarget;
	}

	EHumanoidBones CAnimationChannel::GetBoneName() const
	{
		return m_BoneName;
	}

	bool CAnimationChannel::Update(const std::vector<float>& Value)
	{
		switch (m_AnimationTarget)
		{
		case EAnimationTarget::TRANSLATION:
			if (!UpdateTranslation(Value)) return false;
			break;
		case EAnimationTarget::ROTATION:
			if (!UpdateRotation(Value)) return false;
			break;
		case EAnimationTarget::SCALE:
			if (!UpdateScale(Value)) return false;
			break;
		case EAnimationTarget::WEIGHTS:
			if (!UpdateWeights(Value)) return false;
			break;
		case EAnimationTarget::MODELMATRIX:
			if (!UpdateModelMatrix(Value)) return false;
			break;
		default:
			break;
		}

		return true;
	}

	bool CAnimationChannel::UpdateTranslation(const std::vector<float>& Value)
	{
		if (Value.size() != 3) return true;

		if (!m_TargetNode) return true;

		m_TargetNode->SetPos(glm::vec3(Value[0], Value[1], Value[2]));

		return true;
	}

	bool CAnimationChannel::UpdateRotation(const std::vector<float>& Value)
	{
		if (Value.size() != 4) return true;

		if (!m_TargetNode) return true;

		// glmのクォータニオンは wxyzで指定する必要がある
		glm::quat quat = glm::quat(Value[3], Value[0], Value[1], Value[2]);

		m_TargetNode->SetRot(quat);

		return true;
	}

	bool CAnimationChannel::UpdateScale(const std::vector<float>& Value)
	{
		if (Value.size() != 3) return true;

		if (!m_TargetNode) return true;

		m_TargetNode->SetScale(glm::vec3(Value[0], Value[1], Value[2]));

		return true;
	}

	bool CAnimationChannel::UpdateWeights(const std::vector<float>& Value)
	{
		// 未実装
		if (!m_TargetNode) return true;

		return true;
	}

	bool CAnimationChannel::UpdateModelMatrix(const std::vector<float>& Value)
	{
		if (Value.size() != 10) return true;

		if (!m_TargetNode) return true;

		m_TargetNode->SetPos(glm::vec3(Value[0], Value[1], Value[2]));

		// glmのクォータニオンは wxyzで指定する必要がある
		glm::quat quat = glm::quat(Value[6], Value[3], Value[4], Value[5]);

		m_TargetNode->SetRot(quat);

		m_TargetNode->SetScale(glm::vec3(Value[7], Value[8], Value[9]));

		return true;
	}
}

#endif // USE_ANIMATION