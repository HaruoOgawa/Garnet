#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>

#ifdef USE_INPUT_SYSTEM
#include "../Input/CInputState.h"
#endif // USE_INPUT_SYSTEM


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

#ifdef USE_INPUT_SYSTEM
		virtual void Update(float SecondsTime, const std::shared_ptr<input::CInputState>& InputState);
#endif // USE_INPUT_SYSTEM

		virtual glm::mat4 GetViewMatrix();

		virtual void SetPos(const glm::vec3& Pos);
		virtual const glm::vec3& GetPos() const;

		virtual void SetCenter(const glm::vec3& Center);
		virtual const glm::vec3& GetCenter() const;
	};
}