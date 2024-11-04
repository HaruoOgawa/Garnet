#ifdef USE_ANIMATION

#include "CSkeleton.h"
#include "../Object/CNode.h"

namespace animation
{
	CSkeleton::CSkeleton(ERigType RigType):
		m_RigType(RigType)
	{
	}

	CSkeleton::~CSkeleton()
	{
	}

	ERigType CSkeleton::GetRig() const
	{
		return m_RigType;
	}

	void CSkeleton::AddBone(const std::shared_ptr<CBone>& Bone)
	{
		m_BoneList.push_back(std::make_tuple(Bone->GetBoneNode()->GetName(), Bone));
	}

	bool CSkeleton::CalCSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix)
	{
		for (const auto& Bone : m_BoneList)
		{
			const auto& BoneNode = std::get<1>(Bone)->GetBoneNode();

			glm::mat4 BoneWorldMatrix = BoneNode->GetWorldMatrix();

			// InverseBindMatrixは対象のBoneを原点(0, 0, 0)に戻す方向にMeshを移動させるための逆行列
			// 例えばC3DObjectでBoneをデバッグ描画しているDebugSphereは(0, 0, 0)の位置にあると仮定してBoneWorldMatrixを乗算しているのでちゃんと綺麗にBoneの位置に配置されている
			// [注意]
			// InverseBindMatrixはBoneのデフォルトワールドマトリックスの逆行列であることに注意
			// glTFだと事前に計算されたものがBufferに入っているが、FBXだと自分で計算する必要があり、ここで沼った
			// MMDなど他のフォーマットに対応する時にもこれには注意しよう
			// 
			// その後、BoneWorldMatrixをかけることでアニメーションで移動後のBoneの位置にMeshを移動させることができる
			// https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/images/SkeletonBoneMatrices.png
			// https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_020_Skeletons.md#the-Bone-matrices
			const glm::mat4& InverseBindMatrix = BoneNode->GeInverseBindMatrix();

			// ボーンアニメーションに関する注意 ////////////////////////////////
			// ボーンアニメーションにおいてボーンノードのスケールが1以外だとせん断(シアー)が発生しメッシュが変な形に折れ曲がる(平行四辺形みたいにずれる)現象が発生するので注意
			///////////////////////////////////////////////////////////////
			glm::mat4 SkinMatrix = BoneWorldMatrix * InverseBindMatrix;

			// Skeletonを持ってる3DObjectのModelMatrixを適応する
			SkinMatrix = ObjectModelMatrix * SkinMatrix;

			MatrixList.push_back(SkinMatrix);
		}

		return true;
	}

	const std::vector<std::tuple<std::string, std::shared_ptr<CBone>>>& CSkeleton::GetBoneList() const
	{
		return m_BoneList;
	}

	void CSkeleton::MakeHumanoidBoneTable()
	{
		for (const auto& Bone : m_BoneList)
		{
			EHumanoidBones CurrentBoneName = std::get<1>(Bone)->GetBoneName();

			if (CurrentBoneName == animation::EHumanoidBones::None) continue;

			m_BoneTable.emplace(CurrentBoneName, std::get<1>(Bone));
		}
	}

	void CSkeleton::AddHumanoidBone(EHumanoidBones BoneName, const std::shared_ptr<CBone>& Bone)
	{
		m_BoneTable.emplace(BoneName, Bone);
	}

	const std::unordered_map<EHumanoidBones, std::shared_ptr<CBone>>& CSkeleton::GetHumanoidBoneTable() const
	{
		return m_BoneTable;
	}

	std::shared_ptr<CBone> CSkeleton::GetBone(EHumanoidBones BoneName)
	{
		const auto it = m_BoneTable.find(BoneName);
		if (BoneName != animation::EHumanoidBones::None && it != m_BoneTable.end()) return it->second;

		return nullptr;
	}

	// IK
	void CSkeleton::MakeIKBoneList()
	{
		for (const auto& Bone : m_BoneList)
		{
			// IKは重いのでひとまず標準ボーン以外は除外する
			if (std::get<1>(Bone)->GetBoneName() == EHumanoidBones::None) continue;

			// IKParamを持っていればリストに追加する
			if (std::get<1>(Bone)->GetIKParam())
			{
				std::shared_ptr<CIKSolver> IKSolver = std::make_shared<CIKSolver>();
				if (!IKSolver->Create(std::get<1>(Bone), m_BoneList)) continue;

				m_IKSolverList.push_back(IKSolver);

				// m_IKBoneListはあとで消す
				m_IKBoneList.push_back(std::get<1>(Bone));
			}
		}
	}

	bool CSkeleton::SolveIK()
	{
		for (const auto& IKSolver : m_IKSolverList)
		{
			if (!IKSolver->Solve()) return false;
		}

		return true;
	}

	const std::vector<std::shared_ptr<CBone>>& CSkeleton::GetIKBoneList() const
	{
		return m_IKBoneList;
	}

	// 付与ボーン
	void CSkeleton::MakeGrantBoneList()
	{
		for (const auto& Bone : m_BoneList)
		{
			if (std::get<1>(Bone)->IsRotateGrant() || std::get<1>(Bone)->IsMoveGrant())
			{
				m_GrantBoneList.push_back(std::get<1>(Bone));
			}
		}
	}

	const std::vector<std::shared_ptr<CBone>>& CSkeleton::GetGrantBoneList() const
	{
		return m_GrantBoneList;
	}

	void CSkeleton::ResetToDefaultSkeletonLocal()
	{
		for (const auto& Bone : m_BoneList)
		{
			std::get<1>(Bone)->GetBoneNode()->ResetToDefaultLocalTransform();
		}
	}

	std::string CSkeleton::CastHumanoidBonesToString(EHumanoidBones BoneName)
	{
		switch (BoneName)
		{
		case animation::EHumanoidBones::None:
			break;
		case animation::EHumanoidBones::Hips:
			return "Hips";
		case animation::EHumanoidBones::LeftUpperLeg:
			return "LeftUpperLeg";
		case animation::EHumanoidBones::RightUpperLeg:
			return "RightUpperLeg";
		case animation::EHumanoidBones::LeftLowerLeg:
			return "LeftLowerLeg";
		case animation::EHumanoidBones::RightLowerLeg:
			return "RightLowerLeg";
		case animation::EHumanoidBones::LeftFoot:
			return "LeftFoot";
		case animation::EHumanoidBones::RightFoot:
			return "RightFoot";
		case animation::EHumanoidBones::Spine:
			return "Spine";
		case animation::EHumanoidBones::Chest:
			return "Chest";
		case animation::EHumanoidBones::UpperChest:
			return "UpperChest";
		case animation::EHumanoidBones::Neck:
			return "Neck";
		case animation::EHumanoidBones::Head:
			return "Head";
		case animation::EHumanoidBones::LeftShoulder:
			return "LeftShoulder";
		case animation::EHumanoidBones::RightShoulder:
			return "RightShoulder";
		case animation::EHumanoidBones::LeftUpperArm:
			return "LeftUpperArm";
		case animation::EHumanoidBones::RightUpperArm:
			return "RightUpperArm";
		case animation::EHumanoidBones::LeftLowerArm:
			return "LeftLowerArm";
		case animation::EHumanoidBones::RightLowerArm:
			return "RightLowerArm";
		case animation::EHumanoidBones::LeftHand:
			return "LeftHand";
		case animation::EHumanoidBones::RightHand:
			return "RightHand";
		case animation::EHumanoidBones::LeftToes:
			return "LeftToes";
		case animation::EHumanoidBones::RightToes:
			return "RightToes";
		case animation::EHumanoidBones::LeftEye:
			return "LeftEye";
		case animation::EHumanoidBones::RightEye:
			return "RightEye";
		case animation::EHumanoidBones::Jaw:
			return "Jaw";
		case animation::EHumanoidBones::LeftThumbProximal:
			return "LeftThumbProximal";
		case animation::EHumanoidBones::LeftThumbIntermediate:
			return "LeftThumbIntermediate";
		case animation::EHumanoidBones::LeftThumbDistal:
			return "LeftThumbDistal";
		case animation::EHumanoidBones::LeftIndexProximal:
			return "LeftIndexProximal";
		case animation::EHumanoidBones::LeftIndexIntermediate:
			return "LeftIndexIntermediate";
		case animation::EHumanoidBones::LeftIndexDistal:
			return "LeftIndexDistal";
		case animation::EHumanoidBones::LeftMiddleProximal:
			return "LeftMiddleProximal";
		case animation::EHumanoidBones::LeftMiddleIntermediate:
			return "LeftMiddleIntermediate";
		case animation::EHumanoidBones::LeftMiddleDistal:
			return "LeftMiddleDistal";
		case animation::EHumanoidBones::LeftRingProximal:
			return "LeftRingProximal";
		case animation::EHumanoidBones::LeftRingIntermediate:
			return "LeftRingIntermediate";
		case animation::EHumanoidBones::LeftRingDistal:
			return "LeftRingDistal";
		case animation::EHumanoidBones::LeftLittleProximal:
			return "LeftLittleProximal";
		case animation::EHumanoidBones::LeftLittleIntermediate:
			return "LeftLittleIntermediate";
		case animation::EHumanoidBones::LeftLittleDistal:
			return "LeftLittleDistal";
		case animation::EHumanoidBones::RightThumbProximal:
			return "RightThumbProximal";
		case animation::EHumanoidBones::RightThumbIntermediate:
			return "RightThumbIntermediate";
		case animation::EHumanoidBones::RightThumbDistal:
			return "RightThumbDistal";
		case animation::EHumanoidBones::RightIndexProximal:
			return "RightIndexProximal";
		case animation::EHumanoidBones::RightIndexIntermediate:
			return "RightIndexIntermediate";
		case animation::EHumanoidBones::RightIndexDistal:
			return "RightIndexDistal";
		case animation::EHumanoidBones::RightMiddleProximal:
			return "RightMiddleProximal";
		case animation::EHumanoidBones::RightMiddleIntermediate:
			return "RightMiddleIntermediate";
		case animation::EHumanoidBones::RightMiddleDistal:
			return "RightMiddleDistal";
		case animation::EHumanoidBones::RightRingProximal:
			return "RightRingProximal";
		case animation::EHumanoidBones::RightRingIntermediate:
			return "RightRingIntermediate";
		case animation::EHumanoidBones::RightRingDistal:
			return "RightRingDistal";
		case animation::EHumanoidBones::RightLittleProximal:
			return "RightLittleProximal";
		case animation::EHumanoidBones::RightLittleIntermediate:
			return "RightLittleIntermediate";
		case animation::EHumanoidBones::RightLittleDistal:
			return "RightLittleDistal";

		default:
			break;
		}

		return std::string();
	}
}

#endif // USE_ANIMATION