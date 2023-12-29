#pragma once
#ifdef USE_ANIMATION

namespace animation
{
	enum class EHumanoidBones
	{
		None = -1,

        //
        // ŠT—v:
        //     This is the Hips bone.
        Hips = 0,
        //
        // ŠT—v:
        //     This is the Left Upper Leg bone.
        LeftUpperLeg = 1,
        //
        // ŠT—v:
        //     This is the Right Upper Leg bone.
        RightUpperLeg = 2,
        //
        // ŠT—v:
        //     This is the Left Knee bone.
        LeftLowerLeg = 3,
        //
        // ŠT—v:
        //     This is the Right Knee bone.
        RightLowerLeg = 4,
        //
        // ŠT—v:
        //     This is the Left Ankle bone.
        LeftFoot = 5,
        //
        // ŠT—v:
        //     This is the Right Ankle bone.
        RightFoot = 6,
        //
        // ŠT—v:
        //     This is the first Spine bone.
        Spine = 7,
        //
        // ŠT—v:
        //     This is the Chest bone.
        Chest = 8,
        //
        // ŠT—v:
        //     This is the Upper Chest bone.
        UpperChest = 54,
        //
        // ŠT—v:
        //     This is the Neck bone.
        Neck = 9,
        //
        // ŠT—v:
        //     This is the Head bone.
        Head = 10,
        //
        // ŠT—v:
        //     This is the Left Shoulder bone.
        LeftShoulder = 11,
        //
        // ŠT—v:
        //     This is the Right Shoulder bone.
        RightShoulder = 12,
        //
        // ŠT—v:
        //     This is the Left Upper Arm bone.
        LeftUpperArm = 13,
        //
        // ŠT—v:
        //     This is the Right Upper Arm bone.
        RightUpperArm = 14,
        //
        // ŠT—v:
        //     This is the Left Elbow bone.
        LeftLowerArm = 0xF,
        //
        // ŠT—v:
        //     This is the Right Elbow bone.
        RightLowerArm = 0x10,
        //
        // ŠT—v:
        //     This is the Left Wrist bone.
        LeftHand = 17,
        //
        // ŠT—v:
        //     This is the Right Wrist bone.
        RightHand = 18,
        //
        // ŠT—v:
        //     This is the Left Toes bone.
        LeftToes = 19,
        //
        // ŠT—v:
        //     This is the Right Toes bone.
        RightToes = 20,
        //
        // ŠT—v:
        //     This is the Left Eye bone.
        LeftEye = 21,
        //
        // ŠT—v:
        //     This is the Right Eye bone.
        RightEye = 22,
        //
        // ŠT—v:
        //     This is the Jaw bone.
        Jaw = 23,
        //
        // ŠT—v:
        //     This is the left thumb 1st phalange.
        LeftThumbProximal = 24,
        //
        // ŠT—v:
        //     This is the left thumb 2nd phalange.
        LeftThumbIntermediate = 25,
        //
        // ŠT—v:
        //     This is the left thumb 3rd phalange.
        LeftThumbDistal = 26,
        //
        // ŠT—v:
        //     This is the left index 1st phalange.
        LeftIndexProximal = 27,
        //
        // ŠT—v:
        //     This is the left index 2nd phalange.
        LeftIndexIntermediate = 28,
        //
        // ŠT—v:
        //     This is the left index 3rd phalange.
        LeftIndexDistal = 29,
        //
        // ŠT—v:
        //     This is the left middle 1st phalange.
        LeftMiddleProximal = 30,
        //
        // ŠT—v:
        //     This is the left middle 2nd phalange.
        LeftMiddleIntermediate = 0x1F,
        //
        // ŠT—v:
        //     This is the left middle 3rd phalange.
        LeftMiddleDistal = 0x20,
        //
        // ŠT—v:
        //     This is the left ring 1st phalange.
        LeftRingProximal = 33,
        //
        // ŠT—v:
        //     This is the left ring 2nd phalange.
        LeftRingIntermediate = 34,
        //
        // ŠT—v:
        //     This is the left ring 3rd phalange.
        LeftRingDistal = 35,
        //
        // ŠT—v:
        //     This is the left little 1st phalange.
        LeftLittleProximal = 36,
        //
        // ŠT—v:
        //     This is the left little 2nd phalange.
        LeftLittleIntermediate = 37,
        //
        // ŠT—v:
        //     This is the left little 3rd phalange.
        LeftLittleDistal = 38,
        //
        // ŠT—v:
        //     This is the right thumb 1st phalange.
        RightThumbProximal = 39,
        //
        // ŠT—v:
        //     This is the right thumb 2nd phalange.
        RightThumbIntermediate = 40,
        //
        // ŠT—v:
        //     This is the right thumb 3rd phalange.
        RightThumbDistal = 41,
        //
        // ŠT—v:
        //     This is the right index 1st phalange.
        RightIndexProximal = 42,
        //
        // ŠT—v:
        //     This is the right index 2nd phalange.
        RightIndexIntermediate = 43,
        //
        // ŠT—v:
        //     This is the right index 3rd phalange.
        RightIndexDistal = 44,
        //
        // ŠT—v:
        //     This is the right middle 1st phalange.
        RightMiddleProximal = 45,
        //
        // ŠT—v:
        //     This is the right middle 2nd phalange.
        RightMiddleIntermediate = 46,
        //
        // ŠT—v:
        //     This is the right middle 3rd phalange.
        RightMiddleDistal = 47,
        //
        // ŠT—v:
        //     This is the right ring 1st phalange.
        RightRingProximal = 48,
        //
        // ŠT—v:
        //     This is the right ring 2nd phalange.
        RightRingIntermediate = 49,
        //
        // ŠT—v:
        //     This is the right ring 3rd phalange.
        RightRingDistal = 50,
        //
        // ŠT—v:
        //     This is the right little 1st phalange.
        RightLittleProximal = 51,
        //
        // ŠT—v:
        //     This is the right little 2nd phalange.
        RightLittleIntermediate = 52,
        //
        // ŠT—v:
        //     This is the right little 3rd phalange.
        RightLittleDistal = 53,
        //
        // ŠT—v:
        //     This is the Last bone index delimiter.
        Max = 55
	};
}

#endif // USE_ANIMATION