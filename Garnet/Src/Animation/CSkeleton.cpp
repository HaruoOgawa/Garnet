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

	void CSkeleton::MakeHumanoidBoneTable(const std::map<animation::EHumanoidBones, std::string>& HumanoidBoneList)
	{
		for (const auto& Bone : m_BoneList)
		{
			EHumanoidBones CurrentBoneName = std::get<1>(Bone)->GetBoneName();

			if (CurrentBoneName == animation::EHumanoidBones::None) continue;

			m_BoneTable.emplace(CurrentBoneName, std::get<1>(Bone));
		}

		// ボーンテーブルを再構築
		for (const auto& HumanoidBone : HumanoidBoneList)
		{
			EHumanoidBones BoneName = HumanoidBone.first;
			const auto& NodeName = HumanoidBone.second;

			auto it = std::find_if(m_BoneList.begin(), m_BoneList.end(), [&](const auto& Src) { return (std::get<0>(Src) == NodeName); });
			if (it == m_BoneList.end()) continue;

			// ボーンを再設定
			auto Bone = std::get<1>(*it);

			Bone->SetBoneName(BoneName);

			AddHumanoidBone(BoneName, Bone);
		}
	}

	void CSkeleton::AddHumanoidBone(EHumanoidBones BoneName, const std::shared_ptr<CBone>& Bone)
	{
		m_BoneTable.emplace(BoneName, Bone);
	}

	const std::map<EHumanoidBones, std::shared_ptr<CBone>>& CSkeleton::GetHumanoidBoneTable() const
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
	
	EHumanoidBones CSkeleton::CastStringToHumanoidBones(const std::string& BoneNameStr)
	{
		if (BoneNameStr == "Hips")
			return EHumanoidBones::Hips;
		else if (BoneNameStr == "LeftUpperLeg")
			return EHumanoidBones::LeftUpperLeg;
		else if (BoneNameStr == "RightUpperLeg")
			return EHumanoidBones::RightUpperLeg;
		else if (BoneNameStr == "LeftLowerLeg")
			return EHumanoidBones::LeftLowerLeg;
		else if (BoneNameStr == "RightLowerLeg")
			return EHumanoidBones::RightLowerLeg;
		else if (BoneNameStr == "LeftFoot")
			return EHumanoidBones::LeftFoot;
		else if (BoneNameStr == "RightFoot")
			return EHumanoidBones::RightFoot;
		else if (BoneNameStr == "Spine")
			return EHumanoidBones::Spine;
		else if (BoneNameStr == "Chest")
			return EHumanoidBones::Chest;
		else if (BoneNameStr == "UpperChest")
			return EHumanoidBones::UpperChest;
		else if (BoneNameStr == "Neck")
			return EHumanoidBones::Neck;
		else if (BoneNameStr == "Head")
			return EHumanoidBones::Head;
		else if (BoneNameStr == "LeftShoulder")
			return EHumanoidBones::LeftShoulder;
		else if (BoneNameStr == "RightShoulder")
			return EHumanoidBones::RightShoulder;
		else if (BoneNameStr == "LeftUpperArm")
			return EHumanoidBones::LeftUpperArm;
		else if (BoneNameStr == "RightUpperArm")
			return EHumanoidBones::RightUpperArm;
		else if (BoneNameStr == "LeftLowerArm")
			return EHumanoidBones::LeftLowerArm;
		else if (BoneNameStr == "RightLowerArm")
			return EHumanoidBones::RightLowerArm;
		else if (BoneNameStr == "LeftHand")
			return EHumanoidBones::LeftHand;
		else if (BoneNameStr == "RightHand")
			return EHumanoidBones::RightHand;
		else if (BoneNameStr == "LeftToes")
			return EHumanoidBones::LeftToes;
		else if (BoneNameStr == "RightToes")
			return EHumanoidBones::RightToes;
		else if (BoneNameStr == "LeftEye")
			return EHumanoidBones::LeftEye;
		else if (BoneNameStr == "RightEye")
			return EHumanoidBones::RightEye;
		else if (BoneNameStr == "Jaw")
			return EHumanoidBones::Jaw;
		else if (BoneNameStr == "LeftThumbProximal")
			return EHumanoidBones::LeftThumbProximal;
		else if (BoneNameStr == "LeftThumbIntermediate")
			return EHumanoidBones::LeftThumbIntermediate;
		else if (BoneNameStr == "LeftThumbDistal")
			return EHumanoidBones::LeftThumbDistal;
		else if (BoneNameStr == "LeftIndexProximal")
			return EHumanoidBones::LeftIndexProximal;
		else if (BoneNameStr == "LeftIndexIntermediate")
			return EHumanoidBones::LeftIndexIntermediate;
		else if (BoneNameStr == "LeftIndexDistal")
			return EHumanoidBones::LeftIndexDistal;
		else if (BoneNameStr == "LeftMiddleProximal")
			return EHumanoidBones::LeftMiddleProximal;
		else if (BoneNameStr == "LeftMiddleIntermediate")
			return EHumanoidBones::LeftMiddleIntermediate;
		else if (BoneNameStr == "LeftMiddleDistal")
			return EHumanoidBones::LeftMiddleDistal;
		else if (BoneNameStr == "LeftRingProximal")
			return EHumanoidBones::LeftRingProximal;
		else if (BoneNameStr == "LeftRingIntermediate")
			return EHumanoidBones::LeftRingIntermediate;
		else if (BoneNameStr == "LeftRingDistal")
			return EHumanoidBones::LeftRingDistal;
		else if (BoneNameStr == "LeftLittleProximal")
			return EHumanoidBones::LeftLittleProximal;
		else if (BoneNameStr == "LeftLittleIntermediate")
			return EHumanoidBones::LeftLittleIntermediate;
		else if (BoneNameStr == "LeftLittleDistal")
			return EHumanoidBones::LeftLittleDistal;
		else if (BoneNameStr == "RightThumbProximal")
			return EHumanoidBones::RightThumbProximal;
		else if (BoneNameStr == "RightThumbIntermediate")
			return EHumanoidBones::RightThumbIntermediate;
		else if (BoneNameStr == "RightThumbDistal")
			return EHumanoidBones::RightThumbDistal;
		else if (BoneNameStr == "RightIndexProximal")
			return EHumanoidBones::RightIndexProximal;
		else if (BoneNameStr == "RightIndexIntermediate")
			return EHumanoidBones::RightIndexIntermediate;
		else if (BoneNameStr == "RightIndexDistal")
			return EHumanoidBones::RightIndexDistal;
		else if (BoneNameStr == "RightMiddleProximal")
			return EHumanoidBones::RightMiddleProximal;
		else if (BoneNameStr == "RightMiddleIntermediate")
			return EHumanoidBones::RightMiddleIntermediate;
		else if (BoneNameStr == "RightMiddleDistal")
			return EHumanoidBones::RightMiddleDistal;
		else if (BoneNameStr == "RightRingProximal")
			return EHumanoidBones::RightRingProximal;
		else if (BoneNameStr == "RightRingIntermediate")
			return EHumanoidBones::RightRingIntermediate;
		else if (BoneNameStr == "RightRingDistal")
			return EHumanoidBones::RightRingDistal;
		else if (BoneNameStr == "RightLittleProximal")
			return EHumanoidBones::RightLittleProximal;
		else if (BoneNameStr == "RightLittleIntermediate")
			return EHumanoidBones::RightLittleIntermediate;
		else if (BoneNameStr == "RightLittleDistal")
			return EHumanoidBones::RightLittleDistal;

		return EHumanoidBones::None;
	}
}

#endif // USE_ANIMATION