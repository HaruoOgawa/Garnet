#include "CNode.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IRenderer.h"

namespace node
{
	CNode::CNode()
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