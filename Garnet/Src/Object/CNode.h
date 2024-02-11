#pragma once

#include <memory>
#include <vector>
#include <string>
#include "../Math/CTransform.h"

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

		void SetLocalTransform(const std::shared_ptr<math::CTransform>& LocalTransform);
		const std::shared_ptr<math::CTransform>& GetLocalTransform() const;

		glm::mat4 GetLocalMatrix() const;

		void SetWorldMatrix(const glm::mat4& WorldMatrix);
		const glm::mat4& GetWorldMatrix() const;
		glm::mat4 CalcWorldMatrix(const glm::mat4& LocalMatrix);

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