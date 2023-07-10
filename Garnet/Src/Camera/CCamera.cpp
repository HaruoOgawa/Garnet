#include "CCamera.h"

namespace camera
{
	CCamera::CCamera():
		m_Pos(0.0f, 0.0f, 1.0f),
		m_Center(0.0f, 0.0f, 0.0f),
		m_Up(0.0f, 1.0f, 0.0f)
	{
	}

#ifdef USE_INPUT_SYSTEM
	void CCamera::Update(float SecondsTime, const std::shared_ptr<input::CInputState>& InputState)
	{
	}
#endif // USE_INPUT_SYSTEM

	glm::mat4 CCamera::GetViewMatrix()
	{
		glm::mat4 vmat = glm::lookAt(m_Pos, m_Center, m_Up);

		return vmat;
	}

	glm::vec3 CCamera::GetViewDir() const
	{
		glm::vec3 ViewDir = glm::normalize(m_Center - m_Pos);

		return ViewDir;
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
}