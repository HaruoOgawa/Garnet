#include "C3DObject.h"

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

	bool C3DObject::Create(api::IGraphicsAPI* pGraphicsAPI)
	{
		// Material
		for (auto& Material : m_MaterialList)
		{
			if (!Material->Create(pGraphicsAPI)) return false;
		}

		// Primitive
		for (const auto& Node : m_NodeList)
		{
			const auto& Mesh = Node->GetMesh();

			for (const auto& Primitive : Mesh->GetPrimitiveList())
			{
				if (!Primitive->Create(pGraphicsAPI, m_MaterialList)) return false;
			}
		}

		return true;
	}

	bool C3DObject::Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection)
	{
		for (auto& Material : m_MaterialList)
		{
			if (!Material->Update(SecondsTime, Camera, Projection)) return false;
		}

		return true;
	}

	bool C3DObject::Draw()
	{
		for (const auto& Node : m_NodeList)
		{
			const auto& Mesh = Node->GetMesh();
			for (const auto& Primitive : Mesh->GetPrimitiveList())
			{
				int MaterialIndex = Primitive->GetMaterialIndex();
				if (MaterialIndex < 0 || MaterialIndex >= m_MaterialList.size()) continue;

				const auto& Material = m_MaterialList[MaterialIndex];
				
				const auto& ModelMatrix = Node->GetTransform()->GetModelMatrix();
				Material->SetUniformValue("model", &ModelMatrix[0][0]);

				if (!Primitive->Draw(Material)) return false;
			}
		}

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

	const std::vector<std::shared_ptr<graphics::CMaterial>>& C3DObject::GetMaterialList() const
	{
		return m_MaterialList;
	}
}