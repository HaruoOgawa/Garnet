#ifdef USE_MMD
#include "CVMDImporter.h"
#include "CVMDData.h"

namespace mmd
{
	bool CVMDImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList)
	{
		CVMDData vmd;

		return true;
	}
}
#endif