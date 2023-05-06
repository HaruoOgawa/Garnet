#include "CNode.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"
#include "../Math/CTransform.h"

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

	bool CNode::Create()
	{
		return true;
	}

	bool CNode::Update(float SecondsTime)
	{
		return true;
	}

	bool CNode::Draw()
	{
		return true;
	}
}