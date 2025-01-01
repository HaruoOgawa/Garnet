#include "CCamera.h"

namespace camera
{
	CCamera::CCamera():
		m_Pos(0.0f, 0.0f, 1.0f),
		m_Center(0.0f, 0.0f, 0.0f),
		m_UpVector(0.0f, 1.0f, 0.0f)
	{
	}

	void CCamera::Update(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState)
	{
	}

	glm::mat4 CCamera::GetViewMatrix()
	{
		glm::mat4 vmat = glm::lookAt(m_Pos, m_Center, m_UpVector);

		return vmat;
	}

	glm::vec3 CCamera::GetViewDir() const
	{
		glm::vec3 ViewDir = glm::normalize(m_Center - m_Pos);

		return ViewDir;
	}

	float CCamera::GetViewLength() const
	{
		return glm::length(m_Center - m_Pos);
	}

	void CCamera::SetPos(const glm::vec3& Pos)
	{
		m_Pos = Pos;
	}

	const glm::vec3& CCamera::GetPos() const
	{
		return m_Pos;
	}

	void CCamera::SetCenter(const glm::vec3& Center)
	{
		m_Center = Center;
	}

	const glm::vec3& CCamera::GetCenter() const
	{
		return m_Center;
	}

	void CCamera::SetUpVector(const glm::vec3& UpVector)
	{
		m_UpVector = UpVector;
	}

	const glm::vec3& CCamera::GetUpVector() const
	{
		return m_UpVector;
	}
}