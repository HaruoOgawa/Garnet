#include "CProjection.h"

namespace projection
{
	CProjection::CProjection():
		m_FOV(45.0f),
		m_ScreenResolution(glm::vec2(1.0f)),
		m_Near(0.1f),
		m_Far(1000.0f)
	{
	}

	glm::mat4 CProjection::GetPrejectionMatrix()
	{
		float Aspect = m_ScreenResolution.x / m_ScreenResolution.y;

		glm::mat4 pmat = glm::perspective(glm::radians(m_FOV), Aspect, m_Near, m_Far);
#ifndef USE_OPENGL
		pmat[1][1] *= -1.0f; /// Yç¿ïWÇÃå¸Ç´ÇîΩì]ÅBVulkan(WebGPU)Ç∆OpenGLÇÕãtÇ»ÇÃÇ©Ç»ÅH
#endif // !USE_OPENGL

		return pmat;
	}

	void CProjection::SetFOV(float FOV)
	{
		m_FOV = FOV;
	}

	float CProjection::GetFOV() const
	{
		return m_FOV;
	}

	void CProjection::SetScreenResolution(int Width, int Height)
	{
		m_ScreenResolution = glm::vec2(static_cast<float>(Width), static_cast<float>(Height));
	}

	const glm::vec2& CProjection::GetScreenResolution() const
	{
		return m_ScreenResolution;
	}

	void CProjection::SetNear(float Near)
	{
		m_Near = Near;
	}

	float CProjection::GetNear() const
	{
		return m_Near;
	}

	void CProjection::SetFar(float Far)
	{
		m_Far = Far;
	}

	float CProjection::GetFar() const
	{
		return m_Far;
	}
}