#pragma once

#ifdef USE_PHYSICS
#include "EPhysicsType.h"

namespace physics
{
	struct SRigidbodyParam
	{
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