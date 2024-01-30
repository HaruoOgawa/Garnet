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

		glm::vec2 X_Interpolation_A;
		glm::vec2 X_Interpolation_B;
		glm::vec2 Y_Interpolation_A;
		glm::vec2 Y_Interpolation_B;
		glm::vec2 Z_Interpolation_A;
		glm::vec2 Z_Interpolation_B;
		glm::vec2 R_Interpolation_A;
		glm::vec2 R_Interpolation_B;
	};
}
#endif