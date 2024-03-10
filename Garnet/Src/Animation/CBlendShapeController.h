#pragma once

#ifdef USE_ANIMATION
#include <vector>
#include <set>
#include <memory>

#include "CBlendShapeClip.h"

namespace object { class CNode; }

namespace animation
{
	class CBlendShapeController
	{
		std::set<std::string> m_PlayingBlendShapeSet;

		std::map<std::string, std::shared_ptr<animation::CBlendShapeClip>> m_BlendShapeClipMap;

		std::vector<std::shared_ptr<object::CNode>> m_MorphNodeList;

		std::vector<std::map<EBlendShapeName, float>> m_MorphWeightsList;
	private:
		void Reset();

		void ResetNodeWeights();
		void ApplyNodeWeights();
	public:
		CBlendShapeController();
		virtual ~CBlendShapeController();

		void AddBlendShapeClip(const std::shared_ptr<animation::CBlendShapeClip>& Clip, const std::string& MotionName, bool IsLoop);

		void AddBlendShapeNode(const std::shared_ptr<object::CNode>& Node);

		bool Update(float DeltaSecondsTime);

		void PlayBlendShape(const std::string& MotionName);
		void StopBlendShape(const std::string& MotionName);
	};
}
#endif