#pragma once
#ifdef USE_ANIMATION

namespace animation
{
	enum class EBlendShapeName
	{
		None = -1,

		Neutral = 0,
		Blink = 1,
		Blink_L = 2,
		Blink_R = 3,
		Joy = 4, // 喜び
		Angry = 5,
		Sorrow = 6,
		Fun = 7, // 楽しみ
		A = 8,
		I = 9,
		U = 10,
		E = 11,
		O = 12,

		// 任意シェイプ
		Optional_Shape_0 = 13,
		Optional_Shape_1 = 14,
		Optional_Shape_2 = 15,
		Optional_Shape_3 = 16,
		Optional_Shape_4 = 17,
		Optional_Shape_5 = 18,
		Optional_Shape_6 = 19,
		Optional_Shape_7 = 20,
		Optional_Shape_8 = 21,
		Optional_Shape_9 = 22,
		Optional_Shape_10 = 23,
		Optional_Shape_11 = 24,
		Optional_Shape_12 = 25,
		Optional_Shape_13 = 26,
		Optional_Shape_14 = 27,
		Optional_Shape_15 = 28,
		Optional_Shape_16 = 29,
		Optional_Shape_17 = 30,

		Max = 31,
	};
}

#endif