#pragma once

#ifdef USE_PHYSICS
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace physics
{
	class IPhysicsObject
	{
	public:
		virtual glm::mat4 GetCurrentPhysicsWorldMatrix(const glm::vec3& Scale) = 0;
		virtual glm::vec3 GetCurrentWorldPos() = 0;
		virtual glm::quat GetCurrentWorldRotate() = 0;
	};
}
#endif