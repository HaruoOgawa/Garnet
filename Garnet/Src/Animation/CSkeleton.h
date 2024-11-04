#pragma once
#ifdef USE_ANIMATION

// 用語を整理すると、Boneは関節(点)でBoneとBoneの繋がりがBone(線)
// Boneを組み合わせた全体構造がSkeleton もしくは Skeleton
// しかしglTFのSkeletonプロパティはアニメーションのルートを示すもので、なくてもルートはわかるものなので

#include <memory>
#include <vector>
#include <map>
#include <tuple>
#include <glm/glm.hpp>

#include "CBone.h"
#include "CIKSolver.h"
#include "ERigType.h"
#include "EHumanoidBones.h"

namespace animation
{
	class CSkeleton
	{
		const ERigType m_RigType;

		std::vector<std::tuple<std::string, std::shared_ptr<CBone>>> m_BoneList;
		
		std::map<EHumanoidBones, std::shared_ptr<CBone>> m_BoneTable;

		// IK
		std::vector<std::shared_ptr<CIKSolver>> m_IKSolverList;
		// 後で消す
		std::vector<std::shared_ptr<CBone>> m_IKBoneList;

		// 付与ボーンリスト
		std::vector<std::shared_ptr<CBone>> m_GrantBoneList;
	public:
		CSkeleton(ERigType RigType);
		virtual ~CSkeleton();

		ERigType GetRig() const;

		void AddBone(const std::shared_ptr<CBone>& Bone);
		
		bool CalCSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix);

		const std::vector<std::tuple<std::string, std::shared_ptr<CBone>>>& GetBoneList() const;

		void MakeHumanoidBoneTable(const std::map<animation::EHumanoidBones, std::string>& HumanoidBoneList);
		void AddHumanoidBone(EHumanoidBones BoneName, const std::shared_ptr<CBone>& Bone);
		const std::map<EHumanoidBones, std::shared_ptr<CBone>>& GetHumanoidBoneTable() const;
		std::shared_ptr<CBone> GetBone(EHumanoidBones BoneName);

		// IK
		void MakeIKBoneList();
		bool SolveIK();

		const std::vector<std::shared_ptr<CBone>>& GetIKBoneList() const;

		// 付与ボーン
		void MakeGrantBoneList();
		const std::vector<std::shared_ptr<CBone>>& GetGrantBoneList() const;

		//
		void ResetToDefaultSkeletonLocal();

		static std::string CastHumanoidBonesToString(EHumanoidBones BoneName);
		static EHumanoidBones CastStringToHumanoidBones(const std::string& BoneNameStr);
	};
}

#endif // USE_ANIMATION