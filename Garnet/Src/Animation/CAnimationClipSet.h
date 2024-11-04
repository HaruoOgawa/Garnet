#pragma once

#ifdef USE_ANIMATION
#include <vector>
#include <memory>

#include "../Animation/CAnimationClip.h"
#include "../Animation/CBlendShapeClip.h"

namespace api { class IGraphicsAPI; }

namespace animation
{
	class CAnimationClipSet
	{
		std::string m_FileName;

		std::vector<std::shared_ptr<animation::CAnimationClip>> m_AnimationClipList;
		std::vector<std::shared_ptr<animation::CBlendShapeClip>> m_BlendShapeClipList;
	public:
		CAnimationClipSet();
		virtual ~CAnimationClipSet() = default;

		void SetFileName(const std::string& Name);
		const std::string& GetFileName() const;

		bool Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, const std::string& FileName, const std::string& Extension, 
			animation::ERigType RigType);

		std::shared_ptr<animation::CAnimationClip> GetAnimationClip(int Index);
		std::shared_ptr<animation::CBlendShapeClip> GetBlendShapeClip(int Index);
	};
}
#endif