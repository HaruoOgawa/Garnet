#pragma once
#ifdef USE_ANIMATION
#include <vector>
#include <glm/glm.hpp>

namespace animation
{
	struct SIKLink
	{
		// リンクボーンのボーンIndex
		int IKLinkBoneIndex = -1;

		// 角度制限
		bool IsLimitAngle = false;
		glm::vec3 LowerAngle = glm::vec3(0.0f);
		glm::vec3 UpperAngle = glm::vec3(0.0f);
	};

	// CCD-IKに則ったパラメーター
	struct SIKParam
	{
		// IKターゲットボーンのボーンIndex
		int IKTargetBoneIndex = -1;

		// IKループ回数
		int IKLoopCount = 0;

		// IKループ計算時の1回あたりの制限角度(ラジアン角)
		float LimitedAngle = 0.0f;

		// IKリンクリスト
		std::vector<SIKLink> IKLinkList;

	public:
		SIKParam(int Index, int Loop, float Angle, const std::vector<SIKLink>& Link) :
			IKTargetBoneIndex(Index), IKLoopCount(Loop), LimitedAngle(Angle), IKLinkList(Link)
		{
		}
	};
}
#endif