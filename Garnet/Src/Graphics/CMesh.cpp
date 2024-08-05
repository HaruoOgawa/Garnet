#include "CMesh.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include "CMaterial.h"

namespace graphics
{
	CMesh::CMesh()
	{
	}

	CMesh::~CMesh()
	{
	}

	bool CMesh::Create(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::string PassName, std::string DepthPassName)
	{
		if (!CreateBuffer()) return false;

		for (const auto& Primitive : GetPrimitiveList())
		{
			int MaterialIndex = Primitive->GetMaterialIndex();
			if (MaterialIndex < 0 || MaterialIndex >= MaterialList.size()) continue;

			const auto& Material = MaterialList[MaterialIndex];

			if (!Primitive->Create(pGraphicsAPI, PassName, Material, false)) return false;

			if (Material->GetDepthMaterial())
			{
				if (!Primitive->Create(pGraphicsAPI, DepthPassName, Material->GetDepthMaterial(), true)) return false;
			}

			// 生成処理が終わったので不要なリソースを解放する
			Primitive->Release();
		}

		return true;
	}

	bool CMesh::CreateBuffer()
	{
		for (auto& VertexBuffer : m_VertexBufferList)
		{
			if (!VertexBuffer->Create()) return false;
		}

		for (auto& IndexBuffer : m_IndexBufferList)
		{
			if (!IndexBuffer->Create()) return false;
		}

		return true;
	}

	void CMesh::AddVertexBuffer(const std::shared_ptr<CVertexBuffer>& Buffer)
	{
		m_VertexBufferList.push_back(Buffer);
	}

	void CMesh::AddIndexBuffer(const std::shared_ptr<CIndexBuffer>& Buffer)
	{
		m_IndexBufferList.push_back(Buffer);
	}

	void CMesh::AddPrimitive(const std::shared_ptr<CPrimitive>& Primitive)
	{
		m_PrimitiveList.push_back(Primitive);
	}

	const std::vector<std::shared_ptr<CPrimitive>>& CMesh::GetPrimitiveList() const
	{
		return m_PrimitiveList;
	}

	void CMesh::CreatePresetSimpleMesh(const std::shared_ptr<CVertexBuffer>& VertexBuffer, const std::shared_ptr<CIndexBuffer>& IndexBuffer, int MaterialIndex, graphics::EPresetPrimitiveType PresetType)
	{
		AddVertexBuffer(VertexBuffer);
		AddIndexBuffer(IndexBuffer);

		std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(VertexBuffer, IndexBuffer, MaterialIndex);
		Primitive->SetPresetType(PresetType);
		AddPrimitive(Primitive);
	}

	void CMesh::SetMorphDataList(const std::shared_ptr<CPrimitive>& Primitive, int PrimitiveIndex, const std::vector<std::map<int, glm::vec3>>& MorphDataList)
	{
		Primitive->SetUseMorph(true);

		m_MorphDataList.emplace(PrimitiveIndex, MorphDataList);
	}

	const std::map<int, std::vector<std::map<int, glm::vec3>>>& CMesh::GetMorphDataList() const
	{
		return m_MorphDataList;
	}
}