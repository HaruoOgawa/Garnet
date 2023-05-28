#include "CNode.h"
#include "../Graphics/CMaterial.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"

namespace object
{
	CNode::CNode(const std::shared_ptr<graphics::CMesh>& Mesh, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList):
		m_Transform(std::make_shared<math::CTransform>()),
		m_Mesh(Mesh)
	{
		if (m_Mesh)
		{
			for (const auto& Primitive : m_Mesh->GetPrimitiveList())
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

	const std::vector<int>& CNode::GetDynamicOffsetNumList() const
	{
		return m_DynamicOffsetNumList;
	}
}