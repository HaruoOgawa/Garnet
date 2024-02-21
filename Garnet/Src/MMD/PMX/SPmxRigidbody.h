#pragma once

#ifdef USE_MMD

#include <string>
#include <glm/glm.hpp>

namespace mmd
{
	enum class EPmxPhysicsShape
	{
		NONE = -1,
		SPHERE = 0,
		BOX = 1,
		CAPSULE = 2,
	};

	struct SPmxRigidbody
	{
		std::pair<std::string, std::wstring> RigidbodyName = {"", L""};
		std::pair<std::string, std::wstring> RigidbodyNameEN = { "", L"" };
		int RelationBoneIndex = -1;
		unsigned char group = 0;
		unsigned short NoneCollideGroupFlag = 0;
		EPmxPhysicsShape PhysicsShape = EPmxPhysicsShape::NONE;
		glm::vec3 Size = glm::vec3(0.0f);
		glm::vec3 Pos = glm::vec3(0.0f);
		glm::vec3 Rotate = glm::vec3(0.0f);
		float Mass = 0.0f;
		float TransDamping = 0.0f;
		float RotateDamping = 0.0f;
		float Repulsion = 0.0f;
		float Friction = 0.0f;
		unsigned char PhysicsType = 0;
	};
}
#endif