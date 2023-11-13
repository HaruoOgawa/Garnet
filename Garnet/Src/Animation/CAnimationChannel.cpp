#include "CAnimationChannel.h"
#include "../Object/CNode.h"

namespace animation
{
	CAnimationChannel::CAnimationChannel(int SamplerIndex, EAnimationTarget AnimationTarget, const std::shared_ptr<object::CNode>& TargetNode):
		m_SamplerIndex(SamplerIndex),
		m_AnimationTarget(AnimationTarget),
		m_TargetNode(TargetNode)
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

		m_TargetNode->SetPos(glm::vec3(Value[0], Value[1], Value[2]));

		return true;
	}

	bool CAnimationChannel::UpdateRotation(const std::vector<float>& Value)
	{
		if (Value.size() != 4) return true;

		// glmのクォータニオンは wxyzで指定する必要がある？
		glm::quat quat = glm::quat(Value[3], Value[0], Value[1], Value[2]);

		m_TargetNode->SetRot(quat);

		return true;
	}

	bool CAnimationChannel::UpdateScale(const std::vector<float>& Value)
	{
		if (Value.size() != 3) return true;

		m_TargetNode->SetScale(glm::vec3(Value[0], Value[1], Value[2]));

		return true;
	}

	bool CAnimationChannel::UpdateWeights(const std::vector<float>& Value)
	{
		// 未実装

		return true;
	}

	bool CAnimationChannel::UpdateModelMatrix(const std::vector<float>& Value)
	{
		if (Value.size() != 16) return true;

		glm::mat4 ModelMat = glm::mat4(1.0f);
		std::memcpy(&ModelMat[0][0], &Value[0], sizeof(Value[0]) * Value.size());

		m_TargetNode->GetLocalTransform()->CastModelMatrixToTransform(ModelMat);

		return true;
	}
}