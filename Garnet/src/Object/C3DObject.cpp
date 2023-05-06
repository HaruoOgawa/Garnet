#include "C3DObject.h"
#include "../Graphics/CMaterial.h"

namespace object
{
	C3DObject::C3DObject()
	{

	}

	C3DObject::~C3DObject()
	{
		m_NodeList.clear();
		m_MaterialList.clear();
	}

	bool C3DObject::Update(float Time)
	{
		return true;
	}

	bool C3DObject::Draw()
	{
		return true;
	}

	void C3DObject::AddNode(const std::shared_ptr<CNode>& Node)
	{
		m_NodeList.push_back(Node);
	}

	void C3DObject::AddMaterial(const std::shared_ptr<graphics::CMaterial>& Material)
	{
		m_MaterialList.push_back(Material);
	}
}