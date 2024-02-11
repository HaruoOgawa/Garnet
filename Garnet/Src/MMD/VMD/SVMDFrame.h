#pragma once
#ifdef USE_MMD
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../../Animation/EHumanoidBones.h"

namespace mmd
{
	struct SVMDFrame
	{
		animation::EHumanoidBones BoneName;

		int FrameIndex;

		glm::vec3 Pos;
		glm::quat Rot;

		std::vector<glm::vec2> XPointList;
		std::vector<glm::vec2> YPointList;
		std::vector<glm::vec2> ZPointList;
		std::vector<glm::vec2> RPointList;
	};
}
#endif