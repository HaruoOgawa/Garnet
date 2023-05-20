#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace camera
{
	class CCamera
	{
		glm::vec3 m_Pos;
		glm::vec3 m_Center;
		const glm::vec3 m_Up;
	public:
		CCamera();
		virtual ~CCamera() = default;

		glm::mat4 GetViewMatrix();

		void SetPos(const glm::vec3& Pos);
		const glm::vec3& GetPos() const;

		void SetCenter(const glm::vec3& Center);
		const glm::vec3& GetCenter() const;
	};
}