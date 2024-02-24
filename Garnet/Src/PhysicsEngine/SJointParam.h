#pragma once

#ifdef USE_PHYSICS
#include <glm/glm.hpp>

namespace physics
{
	struct SJointParam
	{
		glm::vec3 Pos6DofBody = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::quat Rotate6DofBody = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 LowerTransLimit = glm::vec3(0.0f);
		glm::vec3 UpperTransLimit = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 LowerRotateLimit = glm::vec3(0.0f);
		glm::vec3 UpperRotateLimit = glm::vec3(3.1415f * 2.0f, 3.1415f * 2.0f, 3.1415f * 2.0f);
		glm::vec3 TransSpring = glm::vec3(0.0f);
		glm::vec3 RotateSpring = glm::vec3(0.0f);
	};
}
#endif