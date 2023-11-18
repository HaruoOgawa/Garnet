#pragma once

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