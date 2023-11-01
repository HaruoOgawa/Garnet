#pragma once

#include <memory>
#include <vector>
#include <string>
#include "../Graphics/CMesh.h"
#include "../Math/CTransform.h"

namespace graphics{ 
	class CMesh; 
	class CMaterial;
}
namespace math { class CTransform; }

namespace object
{
	class CNode
	{
		std::string						  m_Name;

		int								  m_MeshIndex;

		int								  m_SkinIndex;

		std::shared_ptr<math::CTransform> m_LocalTransform;
		glm::mat4						  m_WorldMatrix;
		std::vector<int>                  m_ChildrenNodeIndexList;
		std::vector<int>                  m_DynamicOffsetNumList;

	public:
		CNode(int MeshIndex, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList);
		virtual ~CNode();

		void SetName(const std::string& Name);
		const std::string& GetName() const;

		int GetMeshIndex() const;

		void SetLocalTransform(std::shared_ptr<math::CTransform>& LocalTransform);
		const std::shared_ptr<math::CTransform>& GetLocalTransform() const;

		void SetLocalMatrix(const glm::mat4& LocalMatrix);
		glm::mat4 GetLocalMatrix() const;

		void SetWorldMatrix(const glm::mat4& WorldMatrix);
		const glm::mat4& GetWorldMatrix() const;

		const glm::vec3& GetPos() const;
		void SetPos(const glm::vec3& Pos);

		const glm::quat& GetRot() const;
		void SetRot(const glm::quat& Rot);
		void AddRotate(const glm::vec3& Axis, float Radians);

		const glm::vec3& GetScale() const;
		void SetScale(const glm::vec3& Scale);

		const std::vector<int>& GetChildrenNodeIndexList() const;
		void SetChildrenNodeIndexList(const std::vector<int>& NodeList);

		const std::vector<int>& GetDynamicOffsetNumList() const;

		void SetSkinIndex(int SkinIndex);
		int GetSkinIndex() const;
	};
}