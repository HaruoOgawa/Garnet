#pragma once

#ifdef USE_MMD
namespace mmd
{
	enum class EPmxJointType
	{
		NONE = -1,

		SPRING_6DOF = 0,
		Generic_6DOF = 1,
		P2P = 2,
		ConeTwist = 3,
		Slider = 5,
	};

	struct SPmxJoint
	{
		std::pair<std::string, std::wstring> JointName = { "", L"" };
		std::pair<std::string, std::wstring> JointNameEN = { "", L"" };
		EPmxJointType PmxJointType = EPmxJointType::NONE;
		int BodyAIndex = -1;
		int BodyBIndex = -1;
		glm::vec3 Pos = glm::vec3(0.0f);
		glm::vec3 Rotate = glm::vec3(0.0f);
		glm::vec3 LowwerTransLimit = glm::vec3(0.0f);
		glm::vec3 UpperTransLimit = glm::vec3(0.0f);
		glm::vec3 LowwerRotateLimit = glm::vec3(0.0f);
		glm::vec3 UpperRotateLimit = glm::vec3(0.0f);
		glm::vec3 TransSpring = glm::vec3(0.0f);
		glm::vec3 RotateSpring = glm::vec3(0.0f);
	};
}
#endif