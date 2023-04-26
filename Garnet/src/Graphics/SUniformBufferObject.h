#pragma once
#include <glm/glm.hpp>

namespace renderer
{
	struct SUniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 mvp;
	};
}