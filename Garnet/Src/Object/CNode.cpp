#include "CNode.h"
#include "../Debug/Message/Console.h"

namespace object
{
	CNode::CNode(int MeshIndex, int SelfNodeIndex):
		m_SelfNodeIndex(SelfNodeIndex),
		m_Name(""),
		m_U16Name(L""),
		m_MeshIndex(MeshIndex),
		m_SkeletonIndex(-1),
		m_LocalTransform(std::make_shared<math::CTransform>()),
		m_DefaultLocalTransform(std::make_shared<math::CTransform>()),
		m_PrevLocalTransform(std::make_shared<math::CTransform>()),
		m_WorldMatrix(glm::mat4(1.0f)),
		m_InverseBindMatrix(glm::mat4(1.0f)),
		m_ParentNode(nullptr),
		m_PhysicsObject(nullptr)
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

	// 物理
	void CNode::SetPhysicsObject(const std::shared_ptr<physics::IPhysicsObject>& PhysicsObject)
	{
		m_PhysicsObject = PhysicsObject;
	}

	const std::shared_ptr<physics::IPhysicsObject>& CNode::GetPhysicsObject() const
	{
		return m_PhysicsObject;
	}

	void CNode::CreatePhysicsObject(physics::IPhysicsEngine* pPhysicsEngine)
	{
		// 物理オブジェクトを生成
		if (pPhysicsEngine && m_PhysicsObject)
		{
			glm::vec3 WorldPos = glm::vec3(0.0f);
			glm::quat WorldRotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			glm::vec3 WorldScale = glm::vec3(1.0f);

			math::CTransform::CastModelMatrixToTransform(m_WorldMatrix, WorldPos, WorldRotate, WorldScale);

			// Meshを持っていない物理オブジェクトはボーンなのでサイズは1.0にする
			if (m_MeshIndex == -1)
			{
				WorldScale = glm::vec3(1.0f);
			}

			m_PhysicsObject->Create(pPhysicsEngine, WorldPos, WorldRotate, WorldScale);
		}
	}

	void CNode::ApplyPhysicsConstraint(physics::IPhysicsEngine* pPhysicsEngine)
	{
		// Constraintを反映する
		if (pPhysicsEngine && m_PhysicsObject)
		{
			m_PhysicsObject->ApplyConstraint(pPhysicsEngine);
		}
	}

	void CNode::ApplyPhysicsWorldMatrix()
	{
		// 物理演算の結果を反映する(DynamicObjectのみ)
		if (m_PhysicsObject && !m_PhysicsObject->IsStatic())
		{
			// 物理オブジェクトのワールド座標を渡す
			// 物理オブジェクトに親子関係を持たせるのはConstraints(Joint)を形成するとき(PMXの髪とか服)で、一度物理エンジンにオブジェクトを登録するとConstraints(Joint)の効果で子要素は親要素に自動で引っ張られるようになる
			// なので一度ワールド行列を計算したうえで物理オブジェクトを生成した後は、位置計算を全て物理エンジンに任せる
			m_WorldMatrix = m_PhysicsObject->GetCurrentPhysicsWorldMatrix();
		}
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

		// Staticな物理オブジェクトを持っている時はそれにも位置変更を反映する
		if (m_PhysicsObject && (m_PhysicsObject->IsStatic() || m_PhysicsObject->IsDynamicJoint()))
		{
			glm::vec3 WorldPos = glm::vec3(0.0f);
			glm::quat WorldRotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			glm::vec3 WorldScale = glm::vec3(1.0f);

			math::CTransform::CastModelMatrixToTransform(m_WorldMatrix, WorldPos, WorldRotate, WorldScale);

			// Meshを持っていない物理オブジェクトはボーンなのでサイズは1.0にする
			if (m_MeshIndex == -1)
			{
				WorldScale = glm::vec3(1.0f);
			}

			if (m_PhysicsObject->IsStatic())
			{
				m_PhysicsObject->SetPhysicsWorldTransform(WorldPos, WorldRotate, WorldScale);
			}
			else if (m_PhysicsObject->IsDynamicJoint())
			{
				m_PhysicsObject->UpdateJointWorldTransform(GetPos(), GetRot(), glm::vec3(1.0f));
			}
		}
	}

	const glm::mat4& CNode::GetWorldMatrix() const
	{
		return m_WorldMatrix;
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

	void CNode::SetSkeletonIndex(int SkeletonIndex)
	{
		m_SkeletonIndex = SkeletonIndex;
	}

	int CNode::GetSkeletonIndex() const
	{
		return m_SkeletonIndex;
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