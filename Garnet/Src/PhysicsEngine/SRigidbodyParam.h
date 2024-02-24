#pragma once

#ifdef USE_PHYSICS
#include "EPhysicsType.h"
#include <string>

namespace physics
{
	struct SRigidbodyParam
	{
		std::pair<std::string, std::wstring> RigidbodyName = { "", L"" };
		EPhysicsType PhysicsType = EPhysicsType::STATIC;
		unsigned char group = 0;
		unsigned short NoneCollideGroupFlag = 0;
		float TransDamping = 0.0f;
		float RotateDamping = 0.0f;
		float Repulsion = 0.0f;
		float Friction = 0.5f;
	};
}
#endif