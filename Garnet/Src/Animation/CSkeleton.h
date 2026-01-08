#pragma once
#ifdef USE_ANIMATION

// 用語を整理すると、Boneは関節(点)でBoneとBoneの繋がりがBone(線)
// Boneを組み合わせた全体構造がSkeleton もしくは Skeleton
// しかしglTFのSkeletonプロパティはアニメーションのルートを示すもので、なくてもルートはわかるものなので

#include <memory>
#include <vector>
#include <map>
#include <tuple>
#include <string>
#include <set>
#include <glm/glm.hpp>

#include "CBone.h"
#include "CIKSolver.h"
#include "ERigType.h"
#include "EHumanoidBones.h"

namespace object {
	class C3DObject;
	class CNode;
}

namespace animation
{
	class CSkeleton
	{
		const std::string m_Name;

		ERigType m_RigType;

		std::vector<std::tuple<std::string, std::shared_ptr<CBone>>> m_BoneList;
		
		std::map<EHumanoidBones, std::shared_ptr<CBone>> m_BoneTable;
		std::map<std::shared_ptr<object::CNode>, std::shared_ptr<CBone>> m_NodeBoneMap;

		// IK
		std::vector<std::shared_ptr<CIKSolver>> m_IKSolverList;
		// 後で消す
		std::vector<std::shared_ptr<CBone>> m_IKBoneList;

		// 付与ボーンリスト
		std::vector<std::shared_ptr<CBone>> m_GrantBoneList;

		// ロンリーボーン
		// 標準ボーン・付与ボーン・IKボーン・物理ボーンのどれでもないボーンのリスト
		// ロンリーボーン・追従ボーンのペア
		std::map<std::shared_ptr<CBone>, std::shared_ptr<CBone>> m_LoneryBoneMap;
	
	private:
		bool FindNearestStandardBone(const std::shared_ptr<object::CNode>& Node, std::shared_ptr<CBone>& FollowBone,
			const std::vector<std::shared_ptr<object::CNode>>& NodeList);
	
	public:
		CSkeleton(ERigType RigType, const std::string& Name);
		virtual ~CSkeleton();

		const std::string& GetName() const;

		ERigType GetRig() const;
		void SetRig(ERigType Type);

		void AddBone(const std::shared_ptr<CBone>& Bone);
		
		bool CalCSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix);

		const std::vector<std::tuple<std::string, std::shared_ptr<CBone>>>& GetBoneList() const;

		void MakeHumanoidBoneTable(const std::map<animation::EHumanoidBones, std::string>& HumanoidBoneList);
		void AddHumanoidBone(EHumanoidBones BoneName, const std::shared_ptr<CBone>& Bone);
		const std::map<EHumanoidBones, std::shared_ptr<CBone>>& GetHumanoidBoneTable() const;
		std::shared_ptr<CBone> GetBone(EHumanoidBones BoneName);

		// IK
		void MakeIKBoneList();
		void RemoveIKLoneryBone();
		bool SolveIK();

		const std::vector<std::shared_ptr<CBone>>& GetIKBoneList() const;

		// 付与ボーン
		void MakeGrantBoneList();
		const std::vector<std::shared_ptr<CBone>>& GetGrantBoneList() const;

		// ロンリーボーン
		void MakeLoneryBone(std::set<std::shared_ptr<animation::CBone>>& LoneryBoneSet, object::C3DObject* Object);
		const std::map<std::shared_ptr<CBone>, std::shared_ptr<CBone>>& GetLoneryBoneMap() const;

		//
		void ResetToDefaultSkeletonLocal();

		static std::string CastHumanoidBonesToString(EHumanoidBones BoneName);
		static EHumanoidBones CastStringToHumanoidBones(const std::string& BoneNameStr);
	};
}

#endif // USE_ANIMATION