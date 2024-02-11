#pragma once
#ifdef USE_ANIMATION

namespace animation
{
	enum class EHumanoidBones
	{
		None = -1,
        Hips,
        LeftUpperLeg,
        RightUpperLeg,
        LeftLowerLeg,
        RightLowerLeg,
        LeftFoot,
        RightFoot,
        Spine,
        Chest,
        UpperChest,
        Neck,
        Head,
        LeftShoulder,
        RightShoulder,
        LeftUpperArm,
        RightUpperArm,
        LeftLowerArm,
        RightLowerArm,
        LeftHand,
        RightHand,
        LeftToes,
        RightToes,
        LeftEye,
        RightEye,
        Jaw,
        LeftThumbProximal,
        LeftThumbIntermediate,
        LeftThumbDistal,
        LeftIndexProximal,
        LeftIndexIntermediate,
        LeftIndexDistal,
        LeftMiddleProximal,
        LeftMiddleIntermediate,
        LeftMiddleDistal,
        LeftRingProximal,
        LeftRingIntermediate,
        LeftRingDistal,
        LeftLittleProximal,
        LeftLittleIntermediate,
        LeftLittleDistal,
        RightThumbProximal,
        RightThumbIntermediate,
        RightThumbDistal,
        RightIndexProximal,
        RightIndexIntermediate,
        RightIndexDistal,
        RightMiddleProximal,
        RightMiddleIntermediate,
        RightMiddleDistal,
        RightRingProximal,
        RightRingIntermediate,
        RightRingDistal,
        RightLittleProximal,
        RightLittleIntermediate,
        RightLittleDistal,
       
        // MMD用の追加ボーンリスト ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // センター(MMD用)
        Center,

        // 下半身(MMD用)
        LowerBody,

        // 右足ＩＫ(MMD用)
        RightLegIK,

        // 右つま先ＩＫ(MMD用)
        RightToesIK,

        // 左足ＩＫ(MMD用)
        LeftLegIK,

        // 左つま先ＩＫ(MMD用)
        LeftToesIK,
	};
}

#endif // USE_ANIMATION