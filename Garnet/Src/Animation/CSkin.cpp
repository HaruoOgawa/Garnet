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

	bool CSkin::CalcSkinMatrixList(std::vector<glm::mat4>& MatrixList)
	{
		for (int i = 0; i < m_JointList.size(); i++)
		{
			const auto& Joint = m_JointList[i];
			const auto& JointNode = Joint->GetJointNode();
			const auto& ParentJointNode = JointNode->GetParentNode();

			glm::mat4 JointWorldMatrix = JointNode->GetWorldMatrix();

			// 親要素の逆ワールド行列をかけてJoint座標系に変換
			if (ParentJointNode)
			{
				const auto& ParentInverseWorldMatrix = ParentJointNode->GetInverseWorldMatrix();

				//JointWorldMatrix = ParentInverseWorldMatrix * JointWorldMatrix;
			}

			// InverseBindMatrixは対象のJointを原点に戻す方向にMeshを移動させるための逆行列
			// その後、JointWorldMatrixをかけることでアニメーションで移動後のJointの位置にMeshを移動させることができる
			// https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/images/skinJointMatrices.png
			// https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_020_Skins.md#the-joint-matrices
			const glm::mat4& InverseBindMatrix = JointNode->GeInverseBindMatrix();

			glm::mat4 SkinMatrix = JointWorldMatrix * InverseBindMatrix;

			MatrixList.push_back(SkinMatrix);
		}

		return true;
	}

	const std::vector<std::shared_ptr<CJoint>>& CSkin::GetJointList() const
	{
		return m_JointList;
	}
}