#pragma once

#ifdef USE_PHYSICS
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace physics
{
	class IPhysicsEngine;

	class IPhysicsObject
	{
	public:
		virtual bool Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) = 0;

		virtual bool IsStatic() = 0;

		virtual glm::mat4 GetCurrentPhysicsWorldMatrix() = 0;
		virtual glm::vec3 GetCurrentWorldPos() = 0;
		virtual glm::quat GetCurrentWorldRotate() = 0;
	};
}
#endif