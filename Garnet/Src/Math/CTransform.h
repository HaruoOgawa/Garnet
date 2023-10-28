#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace math
{
	class CTransform
	{
		glm::vec3 m_Pos;
		glm::vec3 m_Rot;
		glm::vec3 m_Scale;
	public:
		CTransform();
		virtual ~CTransform() = default;

		glm::mat4 GetModelMatrix();

		const glm::vec3& GetPos() const;
		void SetPos(const glm::vec3& Pos);

		const glm::vec3& GetRot() const;
		void SetRot(const glm::vec3& Rot);
		void AddRot(const glm::vec3& Rot);

		const glm::vec3& GetScale() const;
		void SetScale(const glm::vec3& Scale);
	};
}