#pragma once

#ifdef USE_PHYSICS
namespace physics
{
	enum class EPhysicsType
	{
		STATIC = 0,
		DYNAMIC = 1,
		DYNAMIC_JOINT = 2,
	};
}
#endif