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
		m_TextureList.clear();
	}

	bool C3DObject::Create(api::IGraphicsAPI* pGraphicsAPI)
	{
		// Material
		for (auto& Material : m_MaterialList)
		{
			if (!Material->Create(m_TextureList)) return false;
		}

		// Primitive
		for (const auto& Node : m_NodeList)
		{
			const auto& Mesh = Node->GetMesh();

			for (const auto& Primitive : Mesh->GetPrimitiveList())
			{
				int MaterialIndex = Primitive->GetMaterialIndex();
				if (MaterialIndex < 0 || MaterialIndex >= m_MaterialList.size()) continue;

				const auto& Material = m_MaterialList[MaterialIndex];

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
			const auto& ModelMatrix = Node->GetTransform()->GetModelMatrix();
			const auto& Mesh = Node->GetMesh();
			const auto& DynamicOffsetList = Node->GetDynamicOffsetNumList();

			if (DynamicOffsetList.size() != Mesh->GetPrimitiveList().size()) continue; // PrimitiveList‚ÆNode‚ÌDynamicOffsetNumList‚Íˆê’v‚µ‚Ä‚¢‚é

			for (int PrimitiveIndex = 0; PrimitiveIndex < Mesh->GetPrimitiveList().size(); PrimitiveIndex++)
			{
				const auto& Primitive = Mesh->GetPrimitiveList()[PrimitiveIndex];

				//
				int MaterialIndex = Primitive->GetMaterialIndex();
				int DynamicOffsetNum = DynamicOffsetList[PrimitiveIndex]; 
				if (MaterialIndex < 0 || MaterialIndex >= m_MaterialList.size()) continue;

				//
				const auto& Material = m_MaterialList[MaterialIndex];
				Material->SetUniformValue("model", &ModelMatrix[0][0], DynamicOffsetNum);

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