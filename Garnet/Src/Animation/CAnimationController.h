#pragma once

#ifdef USE_ANIMATION

#include <string>
#include <unordered_map>

#include "SAnimationLayout.h"
#include "CSkin.h"
#include "CAnimationClip.h"

namespace animation
{
	class CAnimationController
	{
		const float m_MaxBlendingTime;
		float m_CurrBlendingTime;

		std::vector<std::shared_ptr<animation::CAnimationClip>> m_ClipList;
		std::unordered_map<std::string, SAnimationLayout> m_ClipMap;

		std::vector<std::shared_ptr<animation::CSkin>> m_SkinList;

		int m_CurrentClipIndex; // インデックスで使用するアニメーションを選択
		std::string m_CurrentClipName; // Clip名でアニメーションを選択

		int m_TotalJointIndexOffset;
	private:
		void Reset();

		bool ReTargetingRig(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::shared_ptr<CAnimationClip>& DstClip);

		bool BlendMotion(float DeltaSecondsTime);
	public:
		CAnimationController();
		virtual ~CAnimationController();

		bool IsPlayingAnimation();

		bool Update(float DeltaSecondsTime);

		void ChangeMotion(int Index); // インデックス指定でモーションを変更
		void ChangeMotion(const std::string& MotionName); // 名前指定でモーションを変更

		bool CalcSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix);

		void AddMotion(const std::string& MotionName, animation::SAnimationLayout Layout);

		void AddAnimationSkin(const std::shared_ptr<animation::CSkin>& Skin);
		const std::vector<std::shared_ptr<animation::CSkin>>& GetSkinList() const;

		void AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& Clip);
		void AddHumanoidAnimationClip(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::string& MotionName, animation::SAnimationLayout Layout);

		const std::vector<std::shared_ptr<animation::CAnimationClip>>& GetAnimationClipList() const;
		const std::unordered_map<std::string, SAnimationLayout>& GetAnimationClipMap() const;
	};
}

#endif