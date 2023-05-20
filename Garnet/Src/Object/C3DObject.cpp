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
			if (!Material->Create(pGraphicsAPI, m_TextureList)) return false;
		}

		// Primitive
		for (const auto& Node : m_NodeList)
		{
			int MaterialIndex = Node->GetMaterialIndex();
			if (MaterialIndex < 0 || MaterialIndex >= m_MaterialList.size()) continue;

			const auto& Material = m_MaterialList[MaterialIndex];

			const auto& Mesh = Node->GetMesh();

			for (const auto& Primitive : Mesh->GetPrimitiveList())
			{
				if (!Primitive->Create(pGraphicsAPI, Material)) return false;
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
			int MaterialIndex = Node->GetMaterialIndex();
			int DynamicOffsetNum = Node->GetDynamicOffsetNum();
			if (MaterialIndex < 0 || MaterialIndex >= m_MaterialList.size()) continue;

			const auto& Material = m_MaterialList[MaterialIndex];
			const auto& ModelMatrix = Node->GetTransform()->GetModelMatrix();
			Material->SetUniformValue("model", &ModelMatrix[0][0], DynamicOffsetNum);

			const auto& Mesh = Node->GetMesh();

			for (const auto& Primitive : Mesh->GetPrimitiveList())
			{
				if (!Primitive->Draw(Material, DynamicOffsetNum)) return false;
			}
		}

		return true;
	}

	void C3DObject::AddNode(const std::shared_ptr<CNode>& Node)
	{
		m_NodeList.push_back(Node);
	}

	const std::vector<std::shared_ptr<CNode>>& C3DObject::GetNodeList() const
	{
		return m_NodeList;
	}

	void C3DObject::AddMaterial(const std::shared_ptr<graphics::CMaterial>& Material)
	{
		m_MaterialList.push_back(Material);
	}

	const std::vector<std::shared_ptr<graphics::CMaterial>>& C3DObject::GetMaterialList() const
	{
		return m_MaterialList;
	}

	void C3DObject::AddTexture(const std::shared_ptr<graphics::CTexture>& Texture)
	{
		m_TextureList.push_back(Texture);
	}

	const std::vector<std::shared_ptr<graphics::CTexture>>& C3DObject::GetTextureList() const
	{
		return m_TextureList;
	}
}