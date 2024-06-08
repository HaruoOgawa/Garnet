#pragma once

#include <glm/glm.hpp>
#include <string>

namespace physics
{
	struct SJointParam
	{
		std::pair<std::string, std::wstring> JointName = { "", L"" };
		glm::vec3 JointPos = glm::vec3(0.0f);
		glm::vec3 JointRotate = glm::vec3(0.0f);
		glm::vec3 LowerTransLimit = glm::vec3(-1.0f);
		glm::vec3 UpperTransLimit = glm::vec3(1.0f);
		glm::vec3 LowerRotateLimit = glm::vec3(-3.1415f * 2.0f);
		glm::vec3 UpperRotateLimit = glm::vec3(3.1415f * 2.0f);
		glm::vec3 TransSpring = glm::vec3(35.0f, 35.0f, 35.0f);
		glm::vec3 RotateSpring = glm::vec3(35.0f, 35.0f, 35.0f);
	};
}
