#pragma once
#ifdef USE_ANIMATION

#include <string>
#include <memory>
#include "CAnimationClip.h"

namespace animation
{
	struct SAnimationLayout
	{
		std::shared_ptr<CAnimationClip> Clip = nullptr;
		std::string NextClipName = "";
	};
}
#endif