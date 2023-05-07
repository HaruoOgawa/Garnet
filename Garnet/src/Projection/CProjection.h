#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace projection
{
	class CProjection
	{
		float m_FOV;
		float m_Aspect;
		float m_Near;
		float m_Far;
	public:
		CProjection();
		virtual ~CProjection() = default;

		glm::mat4 GetPrejectionMatrix();

		void SetFOV(float FOV);
		float GetFOV() const;

		void SetAspect(float Aspect);
		float GetAspect() const;

		void SetNear(float Near);
		float GetNear() const;

		void SetFar(float Far);
		float GetFar() const;
	};
}