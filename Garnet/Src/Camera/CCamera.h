#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include "../Input/CInputState.h"

namespace camera
{
	class CCamera
	{
	protected:
		glm::vec3 m_Pos;
		glm::vec3 m_Center;
		glm::vec3 m_UpVector;
	public:
		CCamera();
		virtual ~CCamera() = default;

		virtual void Update(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState);

		virtual glm::mat4 GetViewMatrix();

		virtual glm::vec3 GetViewDir() const;
		virtual float GetViewLength() const;

		virtual void SetPos(const glm::vec3& Pos);
		virtual const glm::vec3& GetPos() const;

		virtual void SetCenter(const glm::vec3& Center);
		virtual const glm::vec3& GetCenter() const;
		
		virtual void SetUpVector(const glm::vec3& UpVector);
		virtual const glm::vec3& GetUpVector() const;
	};
}