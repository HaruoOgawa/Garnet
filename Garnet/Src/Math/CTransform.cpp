#include "CTransform.h"

namespace math
{
	CTransform::CTransform():
		m_Pos(0.0f),
		m_Rot(0.0f),
		m_Scale(1.0f)
	{
	}

	glm::mat4 CTransform::GetModelMatrix()
	{
		glm::mat4 trsMatrix = glm::translate(glm::mat4(1.0f), m_Pos);
		glm::mat4 rotMatrix = glm::mat4_cast(glm::quat(m_Rot));
		glm::mat4 sclMatrix = glm::scale(glm::mat4(1.0f), m_Scale);

		glm::mat4 result = trsMatrix * rotMatrix * sclMatrix;

		return result;
	}

	const glm::vec3& CTransform::GetPos() const
	{
		return m_Pos;
	}

	void CTransform::SetPos(const glm::vec3& Pos)
	{
		m_Pos = Pos;
	}

	const glm::vec3& CTransform::GetRot() const
	{
		return m_Rot;
	}

	void CTransform::SetRot(const glm::vec3& Rot)
	{
		m_Rot = Rot;
	}

	void CTransform::AddRot(const glm::vec3& Rot)
	{
		m_Rot += Rot;
	}

	const glm::vec3& CTransform::GetScale() const
	{
		return m_Scale;
	}

	void CTransform::SetScale(const glm::vec3& Scale)
	{
		m_Scale = Scale;
	}
}