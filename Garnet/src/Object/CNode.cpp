#include "CNode.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"

namespace object
{
	CNode::CNode(const std::shared_ptr<graphics::CMesh>& Mesh):
		m_Transform(std::make_shared<math::CTransform>()),
		m_Mesh(Mesh)
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
}