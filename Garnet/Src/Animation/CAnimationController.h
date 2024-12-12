#pragma once

#ifdef USE_ANIMATION

#include <string>
#include <unordered_map>

#include "SAnimationLayout.h"
#include "CSkeleton.h"
#include "CAnimationClip.h"

#include "../Object/CNode.h"

namespace animation
{
	class CAnimationController
	{
		const float m_MaxBlendingTime;
		float m_CurrBlendingTime;
		bool m_SavedPrevTrs;

		std::vector<std::shared_ptr<animation::CAnimationClip>> m_ClipList;
		std::unordered_map<std::string, SAnimationLayout> m_ClipMap;

		int m_CurrentMotionIndex;
		std::string m_CurrentMotionName;
		SAnimationLayout m_CurrentLayout;

		std::shared_ptr<animation::CSkeleton> m_Skeleton;
	private:
		void Reset();

		bool ReTargetRig(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::shared_ptr<CAnimationClip>& TargetClip);

		bool BlendMotion(float DeltaSecondsTime);

		void BlendTranslation(const std::shared_ptr<object::CNode>& Node, float L);
		void BlendRotation(const std::shared_ptr<object::CNode>& Node, float L);

		void CalcWorldMatrix(const glm::mat4& ParentWorldMatrix, const std::shared_ptr<object::CNode>& Node, const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		bool TransferSkeletonAndCopyAnimation(const std::shared_ptr<animation::CAnimationClip>& Src, std::shared_ptr<animation::CAnimationClip>& Dst, bool IsHuman);
	public:
		CAnimationController();
		virtual ~CAnimationController();

		bool IsEnabledSkeleton();
		bool IsPlayingAnimation();

		int GetCurrentMotionIndex() const;
		const std::string& GetCurrentMotionName() const;

		const std::shared_ptr<CAnimationClip>& GetCurrentClip() const;

		bool Update(float DeltaSecondsTime);

		// IKの計算
		bool CalculateIK(const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		// 付与ボーンの計算
		bool CalculateGrantBone(const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		void ChangeMotion(int Index); // インデックス指定でモーションを変更
		void ChangeMotion(const std::string& MotionName); // 名前指定でモーションを変更

		bool CalCSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix);

		void AddMotion(const std::string& MotionName, animation::SAnimationLayout Layout);

		void SetAnimationSkeleton(const std::shared_ptr<animation::CSkeleton>& Skeleton);
		const std::shared_ptr<animation::CSkeleton>& GetSkeleton() const;

		void AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::string& MotionName, animation::SAnimationLayout Layout, bool IsLoop);
		void AddHumanoidAnimationClip(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::string& MotionName, 
			animation::SAnimationLayout Layout, bool IsLoop, bool UseIK);

		const std::vector<std::shared_ptr<animation::CAnimationClip>>& GetAnimationClipList() const;
		const std::unordered_map<std::string, SAnimationLayout>& GetAnimationClipMap() const;
	};
}

#endif