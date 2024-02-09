#include "CNode.h"

namespace object
{
	CNode::CNode(int MeshIndex, int SelfNodeIndex):
		m_SelfNodeIndex(SelfNodeIndex),
		m_Name(""),
		m_U16Name(L""),
		m_MeshIndex(MeshIndex),
		m_SkinIndex(-1),
		m_LocalTransform(std::make_shared<math::CTransform>()),
		m_DefaultLocalTransform(std::make_shared<math::CTransform>()),
		m_PrevLocalTransform(std::make_shared<math::CTransform>()),
		m_WorldMatrix(glm::mat4(1.0f)),
		m_InverseBindMatrix(glm::mat4(1.0f)),
		m_ParentNode(nullptr)
	{
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

	void CNode::SetU16Name(const std::wstring& U16Name)
	{
		m_U16Name = U16Name;
	}
	const std::wstring& CNode::GetU16Name() const
	{
		return m_U16Name;
	}

	int CNode::GetSelfNodeIndex() const
	{
		return m_SelfNodeIndex;
	}

	void CNode::SetMeshIndex(int MeshIndex)
	{
		m_MeshIndex = MeshIndex;
	}

	int CNode::GetMeshIndex() const
	{
		return m_MeshIndex;
	}

	void CNode::SetLocalTransform(const std::shared_ptr<math::CTransform>& LocalTransform)
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

	glm::mat4 CNode::CalcWorldMatrix(const glm::mat4& LocalMatrix)
	{
		glm::mat4 result = LocalMatrix;

		std::shared_ptr<CNode> parentNode = m_ParentNode;
		while (parentNode)
		{
			result = parentNode->GetLocalMatrix() * result;

			parentNode = parentNode->GetParentNode();
		}

		return result;
	}

	void CNode::SetParentNode(const std::shared_ptr<CNode>& ParentNode)
	{
		m_ParentNode = ParentNode;
	}

	const std::shared_ptr<CNode>& CNode::GetParentNode() const
	{
		return m_ParentNode;
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

	void CNode::MulRot(const glm::quat& Rot)
	{
		m_LocalTransform->MulRot(Rot);
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

	// 現在のTransformをデフォルトのTransformとして保存する
	void CNode::SaveAsDefaultLocalTransform()
	{
		m_DefaultLocalTransform->SetPos(m_LocalTransform->GetPos());
		m_DefaultLocalTransform->SetRot(m_LocalTransform->GetRot());
		m_DefaultLocalTransform->SetScale(m_LocalTransform->GetScale());
	}

	const std::shared_ptr<math::CTransform>& CNode::GetDefaultLocalTransform() const
	{
		return m_DefaultLocalTransform;
	}

	glm::mat4 CNode::GetDefaultLocalMatrix() const
	{
		return m_DefaultLocalTransform->GetModelMatrix();
	}

	glm::mat4 CNode::GetDefaultLocalMoveMatrix() const
	{
		const glm::vec3 Pos = m_DefaultLocalTransform->GetPos();
		const glm::mat4 Mat = glm::translate(glm::mat4(1.0f), Pos);

		return Mat;
	}

	glm::mat4 CNode::CalcDefaultWorldMatrix(const glm::mat4& LocalMatrix)
	{
		glm::mat4 result = LocalMatrix;

		std::shared_ptr<CNode> parentNode = m_ParentNode;
		while (parentNode)
		{
			result = parentNode->GetDefaultLocalMatrix() * result;

			parentNode = parentNode->GetParentNode();
		}

		return result;
	}

	glm::mat4 CNode::CalcDefaultParentWorldMatrix()
	{
		glm::mat4 result = glm::mat4(1.0f);

		std::shared_ptr<CNode> parentNode = m_ParentNode;
		while (parentNode)
		{
			result = parentNode->GetDefaultLocalMatrix() * result;

			parentNode = parentNode->GetParentNode();
		}

		return result;
	}

	glm::mat4 CNode::CalcDefaultWorldMoveMatrix(const glm::mat4& MoveMatrix)
	{
		glm::mat4 result = MoveMatrix;

		std::shared_ptr<CNode> parentNode = m_ParentNode;
		while (parentNode)
		{
			result = parentNode->GetDefaultLocalMoveMatrix() * result;

			parentNode = parentNode->GetParentNode();
		}

		return result;
	}

	glm::mat4 CNode::CalcDefaultParentWorldMoveMatrix()
	{
		glm::mat4 result = glm::mat4(1.0f);

		std::shared_ptr<CNode> parentNode = m_ParentNode;
		while (parentNode)
		{
			result = parentNode->GetDefaultLocalMoveMatrix() * result;

			parentNode = parentNode->GetParentNode();
		}

		return result;
	}

	// Transformをデフォルトに戻す
	void CNode::ResetToDefaultLocalTransform()
	{
		m_LocalTransform->SetPos(m_DefaultLocalTransform->GetPos());
		m_LocalTransform->SetRot(m_DefaultLocalTransform->GetRot());
		m_LocalTransform->SetScale(m_DefaultLocalTransform->GetScale());
	}

	// 現在の姿勢を保存する
	void CNode::SavePrevLocalTransform()
	{
		m_PrevLocalTransform->SetPos(m_LocalTransform->GetPos());
		m_PrevLocalTransform->SetRot(m_LocalTransform->GetRot());
		m_PrevLocalTransform->SetScale(m_LocalTransform->GetScale());
	}

	const std::shared_ptr<math::CTransform>& CNode::GetPrevLocalTransform() const
	{
		return m_PrevLocalTransform;
	}

	const std::vector<int>& CNode::GetChildrenNodeIndexList() const
	{
		return m_ChildrenNodeIndexList;
	}

	void CNode::SetChildrenNodeIndexList(const std::vector<int>& NodeList)
	{
		m_ChildrenNodeIndexList = NodeList;
	}

	void CNode::AddChildrenNodeIndex(int Index)
	{
		m_ChildrenNodeIndexList.push_back(Index);
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
}