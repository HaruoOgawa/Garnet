#pragma once
#ifdef USE_ANIMATION

// 用語を整理すると、Boneは関節(点)でBoneとBoneの繋がりがBone(線)
// Boneを組み合わせた全体構造がSkeleton もしくは Skeleton
// しかしglTFのSkeletonプロパティはアニメーションのルートを示すもので、なくてもルートはわかるものなので

#include "CBone.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

namespace animation
{
	class CSkeleton
	{
		std::vector<std::shared_ptr<CBone>> m_BoneList;
		
		std::unordered_map<EHumanoidBones, std::shared_ptr<CBone>> m_BoneTable;

		// IK
		std::vector<std::shared_ptr<CBone>> m_IKBoneList;

		// 付与ボーンリスト
		std::vector<std::shared_ptr<CBone>> m_GrantBoneList;
	public:
		CSkeleton();
		virtual ~CSkeleton();

		void AddBone(const std::shared_ptr<CBone>& Bone);
		
		bool CalCSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix);

		const std::vector<std::shared_ptr<CBone>>& GetBoneList() const;

		void MakeBoneTable();
		const std::unordered_map<EHumanoidBones, std::shared_ptr<CBone>>& GetBoneTable() const;
		std::shared_ptr<CBone> GetBone(EHumanoidBones BoneName);

		// IK
		void MakeIKBoneList();
		const std::vector<std::shared_ptr<CBone>>& GetIKBoneList() const;

		// 付与ボーン
		void MakeGrantBoneList();
		const std::vector<std::shared_ptr<CBone>>& GetGrantBoneList() const;

		//
		void CalCSkeletonWorldMatrix();
		void ResetToDefaultSkeletonLocal();
	};
}

#endif // USE_ANIMATION