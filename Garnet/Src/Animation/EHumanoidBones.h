#pragma once
#ifdef USE_ANIMATION

namespace animation
{
	enum class EHumanoidBones
	{
		None,

		Hips,
		Spine,
		Chest,
		UpperChest,
		Neck,
		Head,
		LeftEye,
		RightEye,
		Jaw,
		LeftShoulder,
		LeftUpperArm,
		LeftLowerArm,
		LeftHand,
		RightShoulder,
		RightUpperArm,
		RightLowerArm,
		RightHand,
		LeftUpperLeg,
		LeftLowerLeg,
		LeftFoot,
		LeftToes,
		RightUpperLeg,
		RightLowerLeg,
		RightFoot,
		RightToes,

		Max,
	};
}

#endif // USE_ANIMATION