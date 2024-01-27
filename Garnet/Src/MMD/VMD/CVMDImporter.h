#pragma once
#ifdef USE_MMD

#include <vector>
#include <memory>

#include "../../Animation/CAnimationClip.h"
#include "../../Interface/IGraphicsAPI.h"

namespace mmd
{
	class CVMDImporter
	{
	public:
		static bool Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList);
	};
}
#endif