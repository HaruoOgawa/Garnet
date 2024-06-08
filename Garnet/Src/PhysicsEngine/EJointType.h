#pragma once

namespace physics
{
	enum class EJointType
	{
		NONE = -1,

		SPRING_6DOF = 0,
		Generic_6DOF = 1,
		P2P = 2,
		ConeTwist = 3,
		Slider = 5,
	};
}