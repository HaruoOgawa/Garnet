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

		// 任意平面に平行なクリッププレーンを使うかどうか
		bool m_UseObliqueMat;
		glm::vec4 m_ClipPlane;

	private:
		glm::mat4 CalculateObliqueMatrix(const glm::mat4& pmat, const glm::vec4& clipPlane);
	public:
		CProjection();
		virtual ~CProjection() = default;

		glm::mat4 GetPerspectiveMatrix();
		glm::mat4 GetPrejectionMatrix();

		void SetFOV(float FOV);
		float GetFOV() const;

		void SetScreenResolution(int Width, int Height);
		const glm::vec2& GetScreenResolution() const;

		void SetNear(float Near);
		float GetNear() const;

		void SetFar(float Far);
		float GetFar() const;

		void EnabledObliqueMat(bool Flag, const glm::vec4& ClipPlane = glm::vec4(0.0f));
	};
}