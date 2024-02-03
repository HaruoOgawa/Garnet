#ifdef USE_MMD
#include "CPmxBone.h"
#include "../../Math/CTransform.h"

namespace mmd
{
	CPmxBone::CPmxBone(const std::pair<std::string, std::wstring>& BoneName, const std::pair<std::string, std::wstring>& BoneName_EN, const glm::vec3& Pos, int ParentBoneIndex, int DeformLayer, unsigned short BoneFlag):
		m_BoneName(BoneName),
		m_BoneName_EN(BoneName_EN),
		m_Pos(Pos),
		m_ParentBoneIndex(ParentBoneIndex),
		m_DeformLayer(DeformLayer),
		m_UseLoacalAxis(false),
		m_LocalAxis(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
	{
		// 必要に応じて随時実装
		ReadBoneFlag(BoneFlag);
	}

	void CPmxBone::ReadBoneFlag(int BoneFlag)
	{
	}

	const std::pair<std::string, std::wstring>& CPmxBone::GetBoneName() const
	{
		return m_BoneName;
	}

	const std::pair<std::string, std::wstring>& CPmxBone::GetBoneName_EN() const
	{
		return m_BoneName_EN;
	}

	const glm::vec3& CPmxBone::GetPos() const
	{
		return m_Pos;
	}

	int CPmxBone::GetParentBoneIndex() const
	{
		return m_ParentBoneIndex;
	}

	int CPmxBone::GetDeformLayer() const
	{
		return m_DeformLayer;
	}

	bool CPmxBone::IsUseLoacalAxis() const
	{
		return m_UseLoacalAxis;
	}

	void CPmxBone::SetLocalAxis(const glm::vec3& XAxisVector, const glm::vec3& ZAxisVector)
	{
		// 用途不明. 計算も見直しが必要
		m_UseLoacalAxis = true;

		glm::vec3 YAxisVector = glm::cross(XAxisVector, ZAxisVector);

		glm::vec3 LocalAxisVector = glm::normalize(XAxisVector + YAxisVector + ZAxisVector);
		glm::vec3 DefaultAxisVector = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f) + glm::vec3(0.0f, 1.0f, 0.0f) + glm::vec3(0.0f, 0.0f, 1.0f));

		m_LocalAxis = glm::normalize(math::CTransform::CalcTwoVectorRotate(DefaultAxisVector, LocalAxisVector));
		//m_LocalAxis = math::CTransform::CalcTwoVectorRotate(glm::vec3(1.0f, 0.0f, 0.0f), XAxisVector) * math::CTransform::CalcTwoVectorRotate(glm::vec3(0.0f, 0.0f, 1.0f), ZAxisVector);
	}

	const glm::quat& CPmxBone::GetLocalAxis() const
	{
		return m_LocalAxis;
	}

	glm::mat4 CPmxBone::GetWorldMatrix()
	{
		glm::mat4 WorldMatrix = glm::mat4(1.0f);

		math::CTransform::CalcModelMatrix(WorldMatrix, m_Pos, m_LocalAxis, false);

		return WorldMatrix;
	}
}
#endif