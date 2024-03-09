#pragma once
#ifdef USE_MMD

#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

#include "../../Animation/CAnimationClip.h"
#include "../../Animation/CBlendShapeClip.h"
#include "../../Interface/IGraphicsAPI.h"

#include "CVMDData.h"
#include "SVMDFrame.h"

namespace mmd
{
	class CVMDImporter
	{
	private:
		static bool CreateAnimationClip(const CVMDData& VMDData, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList);
		static bool CreateBlendShapeClip(const CVMDData& VMDData, std::vector<std::shared_ptr<animation::CBlendShapeClip>>& BlendShapeClipList);
	public:
		static bool Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList, 
			std::vector<std::shared_ptr<animation::CBlendShapeClip>>& BlendShapeClipList);
	};
}
#endif