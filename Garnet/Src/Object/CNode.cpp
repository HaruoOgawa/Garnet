#include "CNode.h"
#include "../Graphics/CMaterial.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"

namespace object
{
	CNode::CNode(int MeshIndex, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList):
		m_Name(""),
		m_MeshIndex(MeshIndex),
		m_SkinIndex(-1),
		m_LocalTransform(std::make_shared<math::CTransform>()),
		m_WorldMatrix(glm::mat4(1.0f)),
		m_InverseBindMatrix(glm::mat4(1.0f)),
		m_ParentNode(nullptr)
	{
		if (MeshIndex >= 0 && MeshIndex < MeshList.size())
		{
			const auto& Mesh = MeshList[MeshIndex];

			for (const auto& Primitive : Mesh->GetPrimitiveList())
			{
				int MaterialIndex = Primitive->GetMaterialIndex();
				if (MaterialIndex < 0 || MaterialIndex >= MaterialList.size())
				{
					// PrimitiveListとDynamicOffsetNumListの順番と数は一致している必要があるのでマテリアルインデックスが無効ならひとまず0を入れておく
					m_DynamicOffsetNumList.push_back(0);
					continue;
				}
				else
				{
					const auto& Material = MaterialList[MaterialIndex];
					Material->IncreaseRefCount();

					int DynamicOffsetNum = Material->GetRefCount();
					m_DynamicOffsetNumList.push_back(DynamicOffsetNum);
				}
			}
		}
	}

	CNode::~CNode()
	{
	}

	void CNode::SetName(const std::string& Name)
	{
		m_Name = Name;
	}

	const std::string& CNode::GetName() const
	{
		return m_Name;
	}

	int CNode::GetMeshIndex() const
	{
		return m_MeshIndex;
	}

	void CNode::SetLocalTransform(std::shared_ptr<math::CTransform>& LocalTransform)
	{
		m_LocalTransform = LocalTransform;
	}

	const std::shared_ptr<math::CTransform>& CNode::GetLocalTransform() const
	{
		return m_LocalTransform;
	}

	glm::mat4 CNode::GetLocalMatrix() const
	{
		return m_LocalTransform->GetModelMatrix();
	}

	void CNode::SetWorldMatrix(const glm::mat4& WorldMatrix)
	{
		m_WorldMatrix = WorldMatrix;
	}

	const glm::mat4& CNode::GetWorldMatrix() const
	{
		return m_WorldMatrix;
	}

	glm::mat4 CNode::GetInverseWorldMatrix() const
	{
		return glm::inverse(m_WorldMatrix);
	}

	const glm::vec3& CNode::GetPos() const
	{
		return m_LocalTransform->GetPos();
	}

	void CNode::SetPos(const glm::vec3& Pos)
	{
		m_LocalTransform->SetPos(Pos);
	}

	const glm::quat& CNode::GetRot() const
	{
		return m_LocalTransform->GetRot();
	}

	void CNode::SetRot(const glm::quat& Rot)
	{
		m_LocalTransform->SetRot(Rot);
	}

	void CNode::AddRotate(const glm::vec3& Axis, float Radians)
	{
		m_LocalTransform->AddRotate(Axis, Radians);
	}

	const glm::vec3& CNode::GetScale() const
	{
		return m_LocalTransform->GetScale();
	}

	void CNode::SetScale(const glm::vec3& Scale)
	{
		m_LocalTransform->SetScale(Scale);
	}

	const std::vector<int>& CNode::GetChildrenNodeIndexList() const
	{
		return m_ChildrenNodeIndexList;
	}

	void CNode::SetChildrenNodeIndexList(const std::vector<int>& NodeList)
	{
		m_ChildrenNodeIndexList = NodeList;
	}

	const std::vector<int>& CNode::GetDynamicOffsetNumList() const
	{
		return m_DynamicOffsetNumList;
	}

	void CNode::SetSkinIndex(int SkinIndex)
	{
		m_SkinIndex = SkinIndex;
	}

	int CNode::GetSkinIndex() const
	{
		return m_SkinIndex;
	}

	void CNode::SetInverseBindMatrix(const glm::mat4& Matrix)
	{
		m_InverseBindMatrix = Matrix;
	}

	const glm::mat4& CNode::GeInverseBindMatrix() const
	{
		return m_InverseBindMatrix;
	}

	void CNode::SetParentNode(const std::shared_ptr<CNode>& ParentNode)
	{
		m_ParentNode = ParentNode;
	}

	const std::shared_ptr<CNode>& CNode::GetParentNode() const
	{
		return m_ParentNode;
	}
}