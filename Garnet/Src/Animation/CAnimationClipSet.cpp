#ifdef USE_ANIMATION
#include "CAnimationClipSet.h"

#if defined(USE_FBX)

#ifdef USE_SMALL_FBX

#include "../FBX/CSmallFBXImporter.h"
#else
#include "../FBX/CFBXImporter.h"
#endif // USE_SMALL_FBX

#endif

#ifdef USE_MMD
#include "../MMD/VMD/CVMDImporter.h"
#endif

namespace animation
{
	CAnimationClipSet::CAnimationClipSet()
	{
	}

	bool CAnimationClipSet::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, const std::string& FileName, const std::string& Extension)
	{
		if (Extension == "fbx")
		{
#if defined(USE_FBX) ///////////////////////////////////////////////////

#ifdef USE_SMALL_FBX
			if (!fbx::CSmallFBXImporter::ImportFBXAnimation(pGraphicsAPI, Data, m_AnimationClipList)) return false;
#else
			if (!fbx::CFBXImporter::ImportFBXAnimation(pGraphicsAPI, FileName, m_AnimationClipList)) return false;
#endif // USE_SMALL_FBX

#endif  ///////////////////////////////////////////////////////////////
		}
		else if (Extension == "vmd")
		{
#ifdef USE_MMD
			if (!mmd::CVMDImporter::Import(pGraphicsAPI, Data, m_AnimationClipList, m_BlendShapeClipList)) return false;
#endif
		}

		return true;
	}

	std::shared_ptr<animation::CAnimationClip> CAnimationClipSet::GetAnimationClip(int Index)
	{
		if (Index < 0 || Index >= m_AnimationClipList.size()) return nullptr;

		return m_AnimationClipList[Index];
	}

	std::shared_ptr<animation::CBlendShapeClip> CAnimationClipSet::GetBlendShapeClip(int Index)
	{
		if (Index < 0 || Index >= m_BlendShapeClipList.size()) return nullptr;

		return m_BlendShapeClipList[Index];
	}
}
#endif