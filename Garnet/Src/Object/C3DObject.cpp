#include "C3DObject.h"

namespace object
{
	C3DObject::C3DObject():
		m_ObjectTransform(std::make_shared<math::CTransform>())
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
		// ワールド行列の計算
		CalcWorldMatrix();

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

	// ワールド行列の初期値を計算(アニメーション等で後々更新される可能性がある)
	void C3DObject::CalcWorldMatrix()
	{
		if (!m_RootNodeIndexList.empty()) 
		{
			// ルートノードから順に走破してワールド行列を計算する
			for (const auto& SceneRootNodeList : m_RootNodeIndexList)
			{
				for (const int RootNodeIndex : SceneRootNodeList)
				{
					if (RootNodeIndex < 0 || RootNodeIndex >= m_NodeList.size()) continue;

					auto& RootNode = m_NodeList[RootNodeIndex];
					const auto& WorldMatrix = RootNode->GetLocalTransform()->GetModelMatrix();

					// ルートなので自身のローカルトランスフォームをワールド行列にする
					RootNode->SetWorldMatrix(WorldMatrix);

					// 子要素の走破をスタートする
					for (const int ChildIndex : RootNode->GetChildrenNodeIndexList())
					{
						if (ChildIndex < 0 || ChildIndex >= m_NodeList.size()) continue;

						auto& ChildNode = m_NodeList[ChildIndex];
						CalcWorldMatrix(ChildNode, WorldMatrix);
					}
				}
			}
		}
		else 
		{
			// ルートノードが指定されていないので自身のローカルトランスフォームをワールド行列として取り扱う
			// ノードの親子関係を構築するにはルートノードと子要素のインデックスの指定が必要である
			for (auto& Node : m_NodeList)
			{
				const auto& WorldMatrix = Node->GetLocalTransform()->GetModelMatrix();
				Node->SetWorldMatrix(WorldMatrix);
			}
		}
	}

	void C3DObject::CalcWorldMatrix(std::shared_ptr<CNode>& Node, const glm::mat4& ParentWorldMatrix)
	{
		// 親要素のワールド行列と自身のローカル行列を乗算して自身のワールド行列を求める
		glm::mat4 WorldMatrix = ParentWorldMatrix * Node->GetLocalTransform()->GetModelMatrix();
		Node->SetWorldMatrix(WorldMatrix);

		// 子要素の走破をスタートする
		for (const int ChildIndex : Node->GetChildrenNodeIndexList())
		{
			if (ChildIndex < 0 || ChildIndex >= m_NodeList.size()) continue;

			auto& ChildNode = m_NodeList[ChildIndex];
			CalcWorldMatrix(ChildNode, WorldMatrix);
		}
	}

	bool C3DObject::Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		for (auto& Material : m_MaterialList)
		{
			if (!Material->Update(SecondsTime, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	bool C3DObject::Draw()
	{
		for (const auto& Node : m_NodeList)
		{
			const auto& WorldMatrix = Node->GetWorldMatrix();
			const auto& Mesh = Node->GetMesh();
			const auto& DynamicOffsetList = Node->GetDynamicOffsetNumList();

			if (DynamicOffsetList.size() != Mesh->GetPrimitiveList().size()) continue; // PrimitiveListとNodeのDynamicOffsetNumListは一致している

			for (int PrimitiveIndex = 0; PrimitiveIndex < Mesh->GetPrimitiveList().size(); PrimitiveIndex++)
			{
				const auto& Primitive = Mesh->GetPrimitiveList()[PrimitiveIndex];

				//
				int MaterialIndex = Primitive->GetMaterialIndex();
				int DynamicOffsetNum = DynamicOffsetList[PrimitiveIndex]; 
				if (MaterialIndex < 0 || MaterialIndex >= m_MaterialList.size()) continue;

				//
				const auto& Material = m_MaterialList[MaterialIndex];
				Material->SetUniformValue("model", &WorldMatrix[0][0], DynamicOffsetNum);

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

	void C3DObject::SetRootNodeIndexList(const std::vector<std::vector<int>>& RootNodeIndexList)
	{
		m_RootNodeIndexList = RootNodeIndexList;
	}

	const std::vector<std::vector<int>>& C3DObject::GetRootNodeIndexList() const
	{
		return m_RootNodeIndexList;
	}
}