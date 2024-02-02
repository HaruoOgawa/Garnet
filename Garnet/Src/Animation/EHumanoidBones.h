#pragma once
#ifdef USE_ANIMATION

namespace animation
{
	enum class EHumanoidBones
	{
		None = -1,

        //
        // äTóv:
        //     This is the Hips bone.
        Hips = 0,
        //
        // äTóv:
        //     This is the Left Upper Leg bone.
        LeftUpperLeg = 1,
        //
        // äTóv:
        //     This is the Right Upper Leg bone.
        RightUpperLeg = 2,
        //
        // äTóv:
        //     This is the Left Knee bone.
        LeftLowerLeg = 3,
        //
        // äTóv:
        //     This is the Right Knee bone.
        RightLowerLeg = 4,
        //
        // äTóv:
        //     This is the Left Ankle bone.
        LeftFoot = 5,
        //
        // äTóv:
        //     This is the Right Ankle bone.
        RightFoot = 6,
        //
        // äTóv:
        //     This is the first Spine bone.
        Spine = 7,
        //
        // äTóv:
        //     This is the Chest bone.
        Chest = 8,
        //
        // äTóv:
        //     This is the Upper Chest bone.
        UpperChest = 54,
        //
        // äTóv:
        //     This is the Neck bone.
        Neck = 9,
        //
        // äTóv:
        //     This is the Head bone.
        Head = 10,
        //
        // äTóv:
        //     This is the Left Shoulder bone.
        LeftShoulder = 11,
        //
        // äTóv:
        //     This is the Right Shoulder bone.
        RightShoulder = 12,
        //
        // äTóv:
        //     This is the Left Upper Arm bone.
        LeftUpperArm = 13,
        //
        // äTóv:
        //     This is the Right Upper Arm bone.
        RightUpperArm = 14,
        //
        // äTóv:
        //     This is the Left Elbow bone.
        LeftLowerArm = 0xF,
        //
        // äTóv:
        //     This is the Right Elbow bone.
        RightLowerArm = 0x10,
        //
        // äTóv:
        //     This is the Left Wrist bone.
        LeftHand = 17,
        //
        // äTóv:
        //     This is the Right Wrist bone.
        RightHand = 18,
        //
        // äTóv:
        //     This is the Left Toes bone.
        LeftToes = 19,
        //
        // äTóv:
        //     This is the Right Toes bone.
        RightToes = 20,
        //
        // äTóv:
        //     This is the Left Eye bone.
        LeftEye = 21,
        //
        // äTóv:
        //     This is the Right Eye bone.
        RightEye = 22,
        //
        // äTóv:
        //     This is the Jaw bone.
        Jaw = 23,
        //
        // äTóv:
        //     This is the left thumb 1st phalange.
        LeftThumbProximal = 24,
        //
        // äTóv:
        //     This is the left thumb 2nd phalange.
        LeftThumbIntermediate = 25,
        //
        // äTóv:
        //     This is the left thumb 3rd phalange.
        LeftThumbDistal = 26,
        //
        // äTóv:
        //     This is the left index 1st phalange.
        LeftIndexProximal = 27,
        //
        // äTóv:
        //     This is the left index 2nd phalange.
        LeftIndexIntermediate = 28,
        //
        // äTóv:
        //     This is the left index 3rd phalange.
        LeftIndexDistal = 29,
        //
        // äTóv:
        //     This is the left middle 1st phalange.
        LeftMiddleProximal = 30,
        //
        // äTóv:
        //     This is the left middle 2nd phalange.
        LeftMiddleIntermediate = 0x1F,
        //
        // äTóv:
        //     This is the left middle 3rd phalange.
        LeftMiddleDistal = 0x20,
        //
        // äTóv:
        //     This is the left ring 1st phalange.
        LeftRingProximal = 33,
        //
        // äTóv:
        //     This is the left ring 2nd phalange.
        LeftRingIntermediate = 34,
        //
        // äTóv:
        //     This is the left ring 3rd phalange.
        LeftRingDistal = 35,
        //
        // äTóv:
        //     This is the left little 1st phalange.
        LeftLittleProximal = 36,
        //
        // äTóv:
        //     This is the left little 2nd phalange.
        LeftLittleIntermediate = 37,
        //
        // äTóv:
        //     This is the left little 3rd phalange.
        LeftLittleDistal = 38,
        //
        // äTóv:
        //     This is the right thumb 1st phalange.
        RightThumbProximal = 39,
        //
        // äTóv:
        //     This is the right thumb 2nd phalange.
        RightThumbIntermediate = 40,
        //
        // äTóv:
        //     This is the right thumb 3rd phalange.
        RightThumbDistal = 41,
        //
        // äTóv:
        //     This is the right index 1st phalange.
        RightIndexProximal = 42,
        //
        // äTóv:
        //     This is the right index 2nd phalange.
        RightIndexIntermediate = 43,
        //
        // äTóv:
        //     This is the right index 3rd phalange.
        RightIndexDistal = 44,
        //
        // äTóv:
        //     This is the right middle 1st phalange.
        RightMiddleProximal = 45,
        //
        // äTóv:
        //     This is the right middle 2nd phalange.
        RightMiddleIntermediate = 46,
        //
        // äTóv:
        //     This is the right middle 3rd phalange.
        RightMiddleDistal = 47,
        //
        // äTóv:
        //     This is the right ring 1st phalange.
        RightRingProximal = 48,
        //
        // äTóv:
        //     This is the right ring 2nd phalange.
        RightRingIntermediate = 49,
        //
        // äTóv:
        //     This is the right ring 3rd phalange.
        RightRingDistal = 50,
        //
        // äTóv:
        //     This is the right little 1st phalange.
        RightLittleProximal = 51,
        //
        // äTóv:
        //     This is the right little 2nd phalange.
        RightLittleIntermediate = 52,
        //
        // äTóv:
        //     This is the right little 3rd phalange.
        RightLittleDistal = 53,
        
        // 54 is UpperChest,
        // 
       
        // ÉZÉìÉ^Å[(MMDóp)
        Center = 55,

        // â∫îºêg(MMDóp)
        LowerBody = 56,

        // âEë´ÇhÇj(MMDóp)
        RightLegIK = 57,

        // âEÇ¬Ç‹êÊÇhÇj(MMDóp)
        RightToesIK = 58,

        // ç∂ë´ÇhÇj(MMDóp)
        LeftLegIK = 59,

        // ç∂Ç¬Ç‹êÊÇhÇj(MMDóp)
        LeftToesIK = 60,

        // 
        // äTóv:
        //     This is the Last bone index delimiter.
        Max = 61
	};
}

#endif // USE_ANIMATION