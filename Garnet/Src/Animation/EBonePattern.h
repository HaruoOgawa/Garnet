#pragma once

#ifdef USE_ANIMATION
namespace animation
{
	enum class EBonePattern
	{
		NONE = -1,

		VRM,
		FBX,
		MIXAMO,
		PMX,
		VMD,

	};
}
#endif