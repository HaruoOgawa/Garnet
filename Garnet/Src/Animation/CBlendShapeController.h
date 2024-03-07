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
	private:
		void Reset(const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		void ResetNodeWeights(const std::vector<std::shared_ptr<object::CNode>>& NodeList);
		void ApplyNodeWeights(const std::vector<float>& MorphWeights, const std::vector<std::shared_ptr<object::CNode>>& NodeList);
	public:
		CBlendShapeController();
		virtual ~CBlendShapeController();

		void AddBlendShapeClip(const std::shared_ptr<animation::CBlendShapeClip>& Clip);

		bool Update(float DeltaSecondsTime, const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		void ChangeBlendShape(int Index, const std::vector<std::shared_ptr<object::CNode>>& NodeList); // インデックス指定でモーションを変更
	};
}
#endif