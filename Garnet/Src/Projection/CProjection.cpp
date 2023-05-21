#include "CProjection.h"

namespace projection
{
	CProjection::CProjection():
		m_FOV(45.0f),
		m_Aspect(1.0f),
		m_Near(0.1f),
		m_Far(1000.0f)
	{
	}

	glm::mat4 CProjection::GetPrejectionMatrix()
	{
		glm::mat4 pmat = glm::perspective(glm::radians(m_FOV), m_Aspect, m_Near, m_Far);
#ifndef __DAWN__
		pmat[1][1] *= -1.0f; /// Yç¿ïWÇÃå¸Ç´ÇîΩì]ÅBVulkanÇ∆OpenGLÇÕãtÇ»ÇÃÇ©Ç»ÅH
#endif
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

	void CProjection::SetAspect(float Aspect)
	{
		m_Aspect = Aspect;
	}

	float CProjection::GetAspect() const
	{
		return m_Aspect;
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