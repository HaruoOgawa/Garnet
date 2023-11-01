#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace math
{
	class CTransform
	{
		glm::vec3 m_Pos;
		glm::quat m_Rot;
		glm::vec3 m_Scale;
	public:
		CTransform();
		virtual ~CTransform() = default;

		glm::mat4 GetModelMatrix();
		void SetModelMatrix(const glm::mat4& ModelMatrix);

		const glm::vec3& GetPos() const;
		void SetPos(const glm::vec3& Pos);

		const glm::quat& GetRot() const;
		void SetRot(const glm::quat& Rot);
		void AddRotate(const glm::vec3& Axis, float Radians);

		const glm::vec3& GetScale() const;
		void SetScale(const glm::vec3& Scale);
	};
}