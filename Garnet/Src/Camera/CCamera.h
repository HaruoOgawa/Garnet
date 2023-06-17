#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace camera
{
	class CCamera
	{
	protected:
		glm::vec3 m_Pos;
		glm::vec3 m_Center;
		const glm::vec3 m_Up;
	public:
		CCamera();
		virtual ~CCamera() = default;

		virtual void Update(float SecondsTime);

		virtual glm::mat4 GetViewMatrix();

		virtual void SetPos(const glm::vec3& Pos);
		virtual const glm::vec3& GetPos() const;

		virtual void SetCenter(const glm::vec3& Center);
		virtual const glm::vec3& GetCenter() const;
	};
}