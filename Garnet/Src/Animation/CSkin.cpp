#include "CSkin.h"
#include "../Object/CNode.h"

namespace animation
{
	CSkin::CSkin()
	{
	}

	CSkin::~CSkin()
	{
	}

	void CSkin::AddJoint(const std::shared_ptr<CJoint>& Joint)
	{
		m_JointList.push_back(Joint);
	}

	bool CSkin::CalcSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix)
	{
		for (int i = 0; i < m_JointList.size(); i++)
		{
			const auto& Joint = m_JointList[i];
			const auto& JointNode = Joint->GetJointNode();

			glm::mat4 JointWorldMatrix = JointNode->GetWorldMatrix();

			// InverseBindMatrixは対象のJointを原点(0, 0, 0)に戻す方向にMeshを移動させるための逆行列
			// 例えばC3DObjectでJointをデバッグ描画しているDebugSphereは(0, 0, 0)の位置にあると仮定してJointWorldMatrixを乗算しているのでちゃんと綺麗にJointの位置に配置されている
			// [注意]
			// InverseBindMatrixはJointのデフォルトワールドマトリックスの逆行列であることに注意
			// glTFだと事前に計算されたものがBufferに入っているが、FBXだと自分で計算する必要があり、ここで沼った
			// MMDなど他のフォーマットに対応する時にもこれには注意しよう
			// 
			// その後、JointWorldMatrixをかけることでアニメーションで移動後のJointの位置にMeshを移動させることができる
			// https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/images/skinJointMatrices.png
			// https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_020_Skins.md#the-joint-matrices
			const glm::mat4& InverseBindMatrix = JointNode->GeInverseBindMatrix();

			glm::mat4 SkinMatrix = JointWorldMatrix * InverseBindMatrix;

			// Skinを持ってる3DObjectのModelMatrixを適応する
			SkinMatrix = ObjectModelMatrix * SkinMatrix;

			MatrixList.push_back(SkinMatrix);
		}

		return true;
	}

	const std::vector<std::shared_ptr<CJoint>>& CSkin::GetJointList() const
	{
		return m_JointList;
	}

	void CSkin::SetJointIndexOffset(int JointIndexOffset)
	{
		m_JointIndexOffset = JointIndexOffset;
	}

	int CSkin::GetJointIndexOffset() const
	{
		return m_JointIndexOffset;
	}
}