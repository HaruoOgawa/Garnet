#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace projection
{
	class CProjection
	{
		float m_FOV;
		glm::vec2 m_ScreenResolution;
		float m_Near;
		float m_Far;
	public:
		CProjection();
		virtual ~CProjection() = default;

		glm::mat4 GetPrejectionMatrix();

		void SetFOV(float FOV);
		float GetFOV() const;

		void SetScreenResolution(int Width, int Height);
		const glm::vec2& GetScreenResolution() const;

		void SetNear(float Near);
		float GetNear() const;

		void SetFar(float Far);
		float GetFar() const;
	};
}