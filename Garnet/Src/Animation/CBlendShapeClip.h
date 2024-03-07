#pragma once

#ifdef USE_ANIMATION
#include <vector>
#include "CAnimationClip.h"
#include "EBlendShapeName.h"

namespace animation
{
	class CBlendShapeClip : public CAnimationClip
	{
		std::vector<EBlendShapeName> m_BlendShapeNameList;

		std::vector<float> m_CurrentMorphWeights;
	public:
		CBlendShapeClip() = default;
		virtual ~CBlendShapeClip() = default;

		void AddAnimationChannel(EBlendShapeName BlendShapeName, const std::shared_ptr<animation::CAnimationChannel>& AnimationChannel);

		const std::vector<EBlendShapeName>& GetBlendShapeNameList() const;

		const std::vector<float>& GetCurrentMorphWeights() const;

		virtual bool UpdateFrame(float CurrentTime) override;
	};
}
#endif