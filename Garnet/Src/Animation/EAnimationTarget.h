#pragma once
#ifdef USE_ANIMATION

namespace animation
{
	enum class EAnimationTarget
	{
		NONE = -1,

		TRANSLATION,
		ROTATION,
		SCALE,
		WEIGHTS,
		MODELMATRIX, // FBX-SDKŒÀ’è
	};
}

#endif // USE_ANIMATION