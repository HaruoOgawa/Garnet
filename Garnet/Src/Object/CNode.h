#pragma once

#include <memory>
#include <vector>
#include <string>
#include "../Math/CTransform.h"
#include "../Interface/IPhysicsObject.h"

namespace object
{
	class CNode
	{
		const int						  m_SelfNodeIndex;

		std::string						  m_Name;
		std::wstring					  m_U16Name;

		int								  m_MeshIndex;

		int								  m_SkeletonIndex;

		std::shared_ptr<math::CTransform> m_LocalTransform;
		std::shared_ptr<math::CTransform> m_DefaultLocalTransform;
		std::shared_ptr<math::CTransform> m_PrevLocalTransform;
		glm::mat4						  m_WorldMatrix;
		std::vector<int>                  m_ChildrenNodeIndexList;

		glm::mat4 m_InverseBindMatrix;

		std::shared_ptr<CNode> m_ParentNode;

		// 物理
		std::vector<std::shared_ptr<physics::IPhysicsObject>> m_PhysicsObjectList;

		// モーフ
		std::vector<float> m_CurrentMorphWeights;

	public:
		CNode(int MeshIndex, int SelfNodeIndex);
		virtual ~CNode();

		int GetSelfNodeIndex() const;

		void SetName(const std::string& Name);
		const std::string& GetName() const;
		
		void SetU16Name(const std::wstring& U16Name);
		const std::wstring& GetU16Name() const;

		void SetMeshIndex(int MeshIndex);
		int GetMeshIndex() const;

		// 物理
		void AddPhysicsObject(const std::shared_ptr<physics::IPhysicsObject>& PhysicsObject);
		const std::vector<std::shared_ptr<physics::IPhysicsObject>>& GetPhysicsObjectList() const;
		void CreatePhysicsObject(physics::IPhysicsEngine* pPhysicsEngine);
		void ApplyPhysicsConstraint(physics::IPhysicsEngine* pPhysicsEngine);

		void ApplyPhysicsWorldMatrix();
		void AlignPhysicsJoint();

		// モーフ
		void ClearMorphWeights();
		void AddMorphWeight(float Weight);
		void SetMorphWeight(const std::vector<float>& MorphWeights);
		const std::vector<float>& GetCurrentMorphWeights() const;

		//
		void SetLocalTransform(const std::shared_ptr<math::CTransform>& LocalTransform);
		const std::shared_ptr<math::CTransform>& GetLocalTransform() const;

		glm::mat4 GetLocalMatrix() const;

		void SetWorldMatrix(const glm::mat4& WorldMatrix);
		const glm::mat4& GetWorldMatrix() const;
		glm::mat4 CalcWorldMatrix();

		void SetParentNode(const std::shared_ptr<CNode>& ParentNode);
		const std::shared_ptr<CNode>& GetParentNode() const;

		const glm::vec3& GetPos() const;
		void SetPos(const glm::vec3& Pos);

		const glm::quat& GetRot() const;
		void SetRot(const glm::quat& Rot);
		void MulRot(const glm::quat& Rot);
		void AddRotate(const glm::vec3& Axis, float Radians);

		const glm::vec3& GetScale() const;
		void SetScale(const glm::vec3& Scale);

		// 現在のTransformをデフォルトのTransformとして保存する
		void SaveAsDefaultLocalTransform();
		const std::shared_ptr<math::CTransform>& GetDefaultLocalTransform() const;
		glm::mat4 GetDefaultLocalMatrix() const;
		glm::mat4 GetDefaultLocalMoveMatrix() const;

		glm::mat4 CalcDefaultWorldMatrix(const glm::mat4& LocalMatrix);
		glm::mat4 CalcDefaultParentWorldMatrix();
		
		glm::mat4 CalcDefaultWorldMoveMatrix(const glm::mat4& MoveMatrix);
		glm::mat4 CalcDefaultParentWorldMoveMatrix();

		// Transformをデフォルトに戻す
		void ResetToDefaultLocalTransform();

		// 現在の姿勢を保存する
		void SavePrevLocalTransform();
		const std::shared_ptr<math::CTransform>& GetPrevLocalTransform() const;

		const std::vector<int>& GetChildrenNodeIndexList() const;
		void SetChildrenNodeIndexList(const std::vector<int>& NodeList);
		void AddChildrenNodeIndex(int Index);

		void SetSkeletonIndex(int SkeletonIndex);
		int GetSkeletonIndex() const;

		void SetInverseBindMatrix(const glm::mat4& Matrix);
		const glm::mat4& GeInverseBindMatrix() const;
	};
}