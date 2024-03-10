#pragma once

#ifdef USE_ANIMATION
#include <vector>
#include <memory>

#include "CBlendShapeClip.h"

namespace object { class CNode; }

namespace animation
{
	class CBlendShapeController
	{
		int m_CurrentClipIndex; // インデックスで使用するアニメーションを選択

		std::vector<std::shared_ptr<animation::CBlendShapeClip>> m_BlendShapeClip;

		std::vector<std::shared_ptr<object::CNode>> m_MorphNodeList;
	private:
		void Reset();

		void ResetNodeWeights();
		void ApplyNodeWeights(const std::map<EBlendShapeName, float>& CurrentMorphWeightMap);
	public:
		CBlendShapeController();
		virtual ~CBlendShapeController();

		void AddBlendShapeClip(const std::shared_ptr<animation::CBlendShapeClip>& Clip, const std::string& MotionName);

		void AddBlendShapeNode(const std::shared_ptr<object::CNode>& Node);

		bool Update(float DeltaSecondsTime);

		void ChangeBlendShape(int Index); // インデックス指定でモーションを変更
	};
}
#endif