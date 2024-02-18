#ifdef USE_ANIMATION

#include "CSkeleton.h"
#include "../Object/CNode.h"

namespace animation
{
	CSkeleton::CSkeleton()
	{
	}

	CSkeleton::~CSkeleton()
	{
	}

	void CSkeleton::AddBone(const std::shared_ptr<CBone>& Bone)
	{
		m_BoneList.push_back(Bone);
	}

	bool CSkeleton::CalCSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix)
	{
		for (int i = 0; i < m_BoneList.size(); i++)
		{
			const auto& Bone = m_BoneList[i];
			const auto& BoneNode = Bone->GetBoneNode();

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

			glm::mat4 SkinMatrix = BoneWorldMatrix * InverseBindMatrix;

			// Skeletonを持ってる3DObjectのModelMatrixを適応する
			SkinMatrix = ObjectModelMatrix * SkinMatrix;

			MatrixList.push_back(SkinMatrix);
		}

		return true;
	}

	const std::vector<std::shared_ptr<CBone>>& CSkeleton::GetBoneList() const
	{
		return m_BoneList;
	}

	void CSkeleton::MakeBoneTable()
	{
		for (const auto& Bone : m_BoneList)
		{
			EHumanoidBones CurrentBoneName = Bone->GetBoneName();

			if (CurrentBoneName != animation::EHumanoidBones::None && m_BoneTable.find(CurrentBoneName) == m_BoneTable.end())
			{
				m_BoneTable.emplace(CurrentBoneName, Bone);
			}
		}
	}

	const std::unordered_map<EHumanoidBones, std::shared_ptr<CBone>>& CSkeleton::GetBoneTable() const
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
			// IKParamを持っていればリストに追加する
			if (Bone->GetIKParam())
			{
				m_IKBoneList.push_back(Bone);
			}
		}
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
			if (Bone->IsRotateGrant() || Bone->IsMoveGrant())
			{
				m_GrantBoneList.push_back(Bone);
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
			Bone->GetBoneNode()->ResetToDefaultLocalTransform();
		}
	}
}

#endif // USE_ANIMATION