#ifdef USE_ANIMATION

#include "CAnimationChannel.h"
#include "../Object/CNode.h"

namespace animation
{
	CAnimationChannel::CAnimationChannel(bool UseAnimLocalAxis, bool TransOffset, int SamplerIndex, EAnimationTarget AnimationTarget,
		const std::string& TargetNodeName, EHumanoidBones BoneName, const std::shared_ptr<object::CNode>& DefaultTargetNode) :
		m_UseAnimLocalAxis(UseAnimLocalAxis),
		m_IsTransOffset(TransOffset),
		m_SamplerIndex(SamplerIndex),
		m_AnimationTarget(AnimationTarget),
		m_TargetNodeName(TargetNodeName),
		m_BoneName(BoneName),
		m_DefaultTargetNode(DefaultTargetNode)
	{
	}

	CAnimationChannel::~CAnimationChannel()
	{
	}

	bool CAnimationChannel::IsUseAnimLocalAxis() const
	{
		return m_UseAnimLocalAxis;
	}

	bool CAnimationChannel::IsTransOffset() const
	{
		return m_IsTransOffset;
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

	std::string CAnimationChannel::GetTargetNodeName() const
	{
		return m_TargetNodeName;
	}

	const std::shared_ptr<object::CNode>& CAnimationChannel::GetDefaultTargetNode() const
	{
		return m_DefaultTargetNode;
	}

	bool CAnimationChannel::Update(const std::vector<float>& Value, const std::shared_ptr<object::CNode>& TargetNode)
	{
		if (!TargetNode) return true;

		switch (m_AnimationTarget)
		{
		case EAnimationTarget::TRANSLATION:
			if (!UpdateTranslation(Value, TargetNode)) return false;
			break;
		case EAnimationTarget::ROTATION:
			if (!UpdateRotation(Value, TargetNode)) return false;
			break;
		case EAnimationTarget::SCALE:
			if (!UpdateScale(Value, TargetNode)) return false;
			break;
		case EAnimationTarget::WEIGHTS:
			if (!UpdateWeights(Value, TargetNode)) return false;
			break;
		case EAnimationTarget::MODELMATRIX:
			if (!UpdateModelMatrix(Value, TargetNode)) return false;
			break;
		default:
			break;
		}

		return true;
	}

	bool CAnimationChannel::UpdateTranslation(const std::vector<float>& Value, const std::shared_ptr<object::CNode>& TargetNode)
	{
		if (Value.size() != 3) return true;

		if (!TargetNode) return true;

		if (m_IsTransOffset)
		{
			// オフセットなので元の座標に加算する
			TargetNode->SetPos(TargetNode->GetDefaultLocalTransform()->GetPos() + glm::vec3(Value[0], Value[1], Value[2]));
		}
		else
		{
			// 座標なので元の座標を置き換える
			TargetNode->SetPos(glm::vec3(Value[0], Value[1], Value[2]));
		}
		

		return true;
	}

	bool CAnimationChannel::UpdateRotation(const std::vector<float>& Value, const std::shared_ptr<object::CNode>& TargetNode)
	{
		if (Value.size() != 4) return true;

		if (!TargetNode) return true;

		// glmのクォータニオンは wxyzで指定する必要がある
		glm::quat quat = glm::quat(Value[3], Value[0], Value[1], Value[2]);

		// ファイルフォーマットによるが、回転は『デフォルトトランスフォームの回転』に『アニメーションデータの回転』を乗算して作られるものである!!!!!!!
		// これが噂によく聞くアニメーションの回転のローカル軸の話である!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// FBXアニメーションにはこれが必要でglTF/VRMアニメーションでは不要である
		// たぶんVRM 1.0からはこのローカル軸がデータに含まれるようになるのかな？
		if (m_UseAnimLocalAxis)
		{
			glm::quat dstQuat = TargetNode->GetDefaultLocalTransform()->GetRot() * quat;

			TargetNode->SetRot(dstQuat);
		}
		else
		{
			glm::quat dstQuat = quat;

			TargetNode->SetRot(dstQuat);
		}

		return true;
	}

	bool CAnimationChannel::UpdateScale(const std::vector<float>& Value, const std::shared_ptr<object::CNode>& TargetNode)
	{
		if (Value.size() != 3) return true;

		if (!TargetNode) return true;

		TargetNode->SetScale(glm::vec3(Value[0], Value[1], Value[2]));

		return true;
	}

	bool CAnimationChannel::UpdateWeights(const std::vector<float>& Value, const std::shared_ptr<object::CNode>& TargetNode)
	{
		if (!TargetNode) return true;

		TargetNode->ClearMorphWeights();

		for (float Weight : Value)
		{
			TargetNode->AddMorphWeight(Weight);
		}

		return true;
	}

	bool CAnimationChannel::UpdateModelMatrix(const std::vector<float>& Value, const std::shared_ptr<object::CNode>& TargetNode)
	{
		if (Value.size() != 10) return true;

		if (!TargetNode) return true;

		if (m_IsTransOffset)
		{
			// オフセットなので元の座標に加算する
			TargetNode->SetPos(TargetNode->GetDefaultLocalTransform()->GetPos() + glm::vec3(Value[0], Value[1], Value[2]));
		}
		else
		{
			// 座標なので元の座標を置き換える
			TargetNode->SetPos(glm::vec3(Value[0], Value[1], Value[2]));
		}

		// glmのクォータニオンは wxyzで指定する必要がある
		glm::quat quat = glm::quat(Value[6], Value[3], Value[4], Value[5]);

		// ファイルフォーマットによるが、回転は『デフォルトトランスフォームの回転』に『アニメーションデータの回転』を乗算して作られるものである!!!!!!!
		// これが噂によく聞くアニメーションの回転のローカル軸の話である!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// FBXアニメーションにはこれが必要でglTF/VRMアニメーションでは不要である
		// たぶんVRM 1.0からはこのローカル軸がデータに含まれるようになるのかな？
		if (m_UseAnimLocalAxis)
		{
			glm::quat dstQuat = TargetNode->GetDefaultLocalTransform()->GetRot() * quat;

			TargetNode->SetRot(dstQuat);
		}
		else
		{
			glm::quat dstQuat = quat;

			TargetNode->SetRot(dstQuat);
		}

		TargetNode->SetScale(glm::vec3(Value[7], Value[8], Value[9]));

		return true;
	}
}

#endif // USE_ANIMATION