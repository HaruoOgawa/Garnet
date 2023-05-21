#include "CNode.h"
#include "../Graphics/CMaterial.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"

namespace object
{
	CNode::CNode(const std::shared_ptr<graphics::CMesh>& Mesh):
		m_Transform(std::make_shared<math::CTransform>()),
		m_Mesh(Mesh),
		m_MaterialIndex(-1),
		m_DynamicOffsetNum(0)
	{
	}

	CNode::~CNode()
	{
	}

	const std::shared_ptr<graphics::CMesh>& CNode::GetMesh() const
	{
		return m_Mesh;
	}

	void CNode::SetTransform(std::shared_ptr<math::CTransform>& Transform)
	{
		m_Transform = Transform;
	}

	const std::shared_ptr<math::CTransform>& CNode::GetTransform() const
	{
		return m_Transform;
	}

	const glm::vec3& CNode::GetPos() const
	{
		return m_Transform->GetPos();
	}

	void CNode::SetPos(const glm::vec3& Pos)
	{
		m_Transform->SetPos(Pos);
	}

	const glm::vec3& CNode::GetRot() const
	{
		return m_Transform->GetRot();
	}

	void CNode::SetRot(const glm::vec3& Rot)
	{
		m_Transform->SetRot(Rot);
	}

	const glm::vec3& CNode::GetScale() const
	{
		return m_Transform->GetScale();
	}

	void CNode::SetScale(const glm::vec3& Scale)
	{
		m_Transform->SetScale(Scale);
	}

	void CNode::LinkMaterialReference(int MaterialIndex, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList)
	{
		if (MaterialIndex < 0 || MaterialIndex >= MaterialList.size()) return;

		const auto& Material = MaterialList[MaterialIndex];
		Material->IncreaseRefCount();

		m_MaterialIndex = MaterialIndex;
		m_DynamicOffsetNum = Material->GetRefCount();
	}

	int CNode::GetMaterialIndex()const 
	{
		return m_MaterialIndex;
	}

	int CNode::GetDynamicOffsetNum() const
	{
		return m_DynamicOffsetNum;
	}
}