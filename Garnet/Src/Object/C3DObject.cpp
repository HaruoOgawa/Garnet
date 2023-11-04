#include "C3DObject.h"

namespace object
{
	C3DObject::C3DObject(const std::string& PassName, const std::string& DepthPassName):
		m_PassName(PassName),
		m_DepthPassName(DepthPassName),
		m_ObjectTransform(std::make_shared<math::CTransform>()),
		m_CurrentClipIndex(-1)
	{
	}

	C3DObject::~C3DObject()
	{
		m_NodeList.clear();
		m_MaterialList.clear();
	}

	bool C3DObject::Create(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment, const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		// GPU上のテクスチャリソースが解放されてしまうので保持しておく
		m_TextureSet = TextureSet;

		// 親ノードを設定
		ApplyParentNode();

		// ワールド行列の計算
		CalcWorldMatrix();

		// Material
		for (auto& Material : m_MaterialList)
		{
			if (!Material->Create(m_TextureSet)) return false;
			
			if (DepthVertex && DepthFragment)
			{
				if (!Material->CreateDepthMaterial(pGraphicsAPI, DepthVertex, DepthFragment)) return false;
			}
		}

		// Primitive
		for (const auto& Mesh : m_MeshList)
		{
			for (const auto& Primitive : Mesh->GetPrimitiveList())
			{
				int MaterialIndex = Primitive->GetMaterialIndex();
				if (MaterialIndex < 0 || MaterialIndex >= m_MaterialList.size()) continue;

				const auto& Material = m_MaterialList[MaterialIndex];

				if (!Primitive->Create(pGraphicsAPI, m_PassName, Material, false)) return false;
				
				if (Material->GetDepthMaterial())
				{
					if (!Primitive->Create(pGraphicsAPI, m_DepthPassName, Material->GetDepthMaterial(), true)) return false;
				}

				// 生成処理が終わったので不要なリソースを解放する
				Primitive->Release();
			}
		}

		return true;
	}

	void C3DObject::ApplyParentNode()
	{
		if (!m_RootNodeIndexList.empty())
		{
			for (const auto& SceneRootNodeList : m_RootNodeIndexList)
			{
				for (const int RootNodeIndex : SceneRootNodeList)
				{
					if (RootNodeIndex < 0 || RootNodeIndex >= m_NodeList.size()) continue;

					auto& RootNode = m_NodeList[RootNodeIndex];

					// 子要素の走破をスタートする
					for (const int ChildIndex : RootNode->GetChildrenNodeIndexList())
					{
						if (ChildIndex < 0 || ChildIndex >= m_NodeList.size()) continue;

						auto& ChildNode = m_NodeList[ChildIndex];
						ApplyParentNode(ChildNode, RootNode);
					}
				}
			}
		}
	}

	void C3DObject::ApplyParentNode(std::shared_ptr<CNode>& Node, const std::shared_ptr<CNode>& ParentNode)
	{
		Node->SetParentNode(ParentNode);

		// 子要素の走破をスタートする
		for (const int ChildIndex : Node->GetChildrenNodeIndexList())
		{
			if (ChildIndex < 0 || ChildIndex >= m_NodeList.size()) continue;

			auto& ChildNode = m_NodeList[ChildIndex];
			ApplyParentNode(ChildNode, Node);
		}
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

	bool C3DObject::Update(float DeltaSecondsTime)
	{
		// アニメーションの計算
		if (IsPlayingAnimation())
		{
			const auto& Clip = m_AnimationClipList[m_CurrentClipIndex];
			if (!Clip->Update(DeltaSecondsTime)) return false;
		}

		// ワールド行列の更新
		// 全ノードマイフレーム更新しているので、そのうちキャッシュを入れて更新は必要なものだけにする
		CalcWorldMatrix();

		return true;
	}

	bool C3DObject::Draw(bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, 
		const std::shared_ptr<object::C3DObject>& DebugSphere)
	{
		// 共通ユニフォームの更新
		for (auto& Material : m_MaterialList)
		{
			if (!Material) continue;
			
			// 共通のユニフォームバッファの更新
			glm::mat4 lightVPMat = DrawInfo->GetLightProjection()->GetPrejectionMatrix() * DrawInfo->GetLightCamera()->GetViewMatrix();

			Material->SetUniformValue("view", &Camera->GetViewMatrix()[0][0]);
			Material->SetUniformValue("proj", &Projection->GetPrejectionMatrix()[0][0]);
			Material->SetUniformValue("lightVPMat", &lightVPMat[0][0]);
			Material->SetUniformValue("lightDir", &DrawInfo->GetLightCamera()->GetViewDir()[0]);
			Material->SetUniformValue("lightColor", &DrawInfo->GetLightColor()[0]);
			Material->SetUniformValue("cameraPos", &Camera->GetPos()[0]);
			Material->SetUniformValue("time", &glm::vec1(DrawInfo->GetSecondsTime())[0]);
			Material->SetUniformValue("deltaTime", &glm::vec1(DrawInfo->GetDeltaSecondsTime())[0]);
			Material->SetUniformValue("useSkinMeshAnimation", &glm::ivec1( (IsPlayingAnimation()? 1 : 0) )[0]);
		}

		for (auto& Material : m_MaterialList)
		{
			if (!Material) continue;

			auto DepthMaterial = Material->GetDepthMaterial();

			if (!DepthMaterial) continue;

			// 共通のユニフォームバッファの更新
			glm::mat4 lightVPMat = DrawInfo->GetLightProjection()->GetPrejectionMatrix() * DrawInfo->GetLightCamera()->GetViewMatrix();
			
			DepthMaterial->SetUniformValue("view", &Camera->GetViewMatrix()[0][0]);
			DepthMaterial->SetUniformValue("proj", &Projection->GetPrejectionMatrix()[0][0]);
			DepthMaterial->SetUniformValue("lightVPMat", &lightVPMat[0][0]);
			DepthMaterial->SetUniformValue("lightDir", &DrawInfo->GetLightCamera()->GetViewDir()[0]);
			DepthMaterial->SetUniformValue("lightColor", &DrawInfo->GetLightColor()[0]);
			DepthMaterial->SetUniformValue("cameraPos", &Camera->GetPos()[0]);
			DepthMaterial->SetUniformValue("time", &glm::vec1(DrawInfo->GetSecondsTime())[0]);
			DepthMaterial->SetUniformValue("deltaTime", &glm::vec1(DrawInfo->GetDeltaSecondsTime())[0]);
			DepthMaterial->SetUniformValue("useSkinMeshAnimation", &glm::ivec1((IsPlayingAnimation() ? 1 : 0))[0]);
		}

		// 描画
		for (const auto& Node : m_NodeList)
		{
			int MeshIndex = Node->GetMeshIndex();
			if (MeshIndex < 0 || MeshIndex >= m_MeshList.size()) continue;

			const auto& WorldMatrix = m_ObjectTransform->GetModelMatrix() * Node->GetWorldMatrix();
			const auto& Mesh = m_MeshList[MeshIndex];
			const auto& DynamicOffsetList = Node->GetDynamicOffsetNumList();

			if (DynamicOffsetList.size() != Mesh->GetPrimitiveList().size()) continue; // PrimitiveListとNodeのDynamicOffsetNumListは一致している

			// SkinMatrixを計算
			std::vector<glm::mat4> SkinMatrixList;
			int SkinIndex = Node->GetSkinIndex();

			if (SkinIndex >= 0 && SkinIndex < m_AnimationSkinList.size() && IsPlayingAnimation())
			{
				const auto& Skin = m_AnimationSkinList[SkinIndex];
				
				if (!Skin->CalcSkinMatrixList(SkinMatrixList, m_ObjectTransform->GetModelMatrix())) return false;
			}

			for (int PrimitiveIndex = 0; PrimitiveIndex < Mesh->GetPrimitiveList().size(); PrimitiveIndex++)
			{
				const auto& Primitive = Mesh->GetPrimitiveList()[PrimitiveIndex];

				int MaterialIndex = Primitive->GetMaterialIndex();
				int DynamicOffsetNum = DynamicOffsetList[PrimitiveIndex];
				if (MaterialIndex < 0 || MaterialIndex >= m_MaterialList.size()) continue;

				std::shared_ptr<graphics::CMaterial> Material = nullptr;

				if (IsDepthPass)
				{
					Material = m_MaterialList[MaterialIndex]->GetDepthMaterial();
				}
				else
				{
					Material = m_MaterialList[MaterialIndex];
				}

				if (!Material) continue;
				
				Material->SetUniformValue("model", &WorldMatrix[0][0], DynamicOffsetNum);

				// SkinMatrixをShaderに渡す
				if (SkinIndex >= 0 && SkinIndex < m_AnimationSkinList.size() && IsPlayingAnimation())
				{
					Material->SetUniformValue("r_SkinMatrixBuffer", &SkinMatrixList[0], DynamicOffsetNum);
				}

				if (!Primitive->Draw(Material, DynamicOffsetNum, IsDepthPass)) return false;
			}
		}
		
		/*for (const auto& Skin : m_AnimationSkinList)
		{
			
			for (const auto& Joint : Skin->GetJointList())
			{
				// Debug用: Jointの描画
				const auto& JointNode = Joint->GetJointNode();
				DebugSphere->SetPos(JointNode->GetWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
				DebugSphere->SetScale(glm::vec3(0.25f));

				if (!DebugSphere->Draw(IsDepthPass, Camera, Projection, DrawInfo)) return false;

				// Debug用: Boneの描画
			}
		}*/

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

	void C3DObject::AddMesh(const std::shared_ptr<graphics::CMesh>& Mesh)
	{
		m_MeshList.push_back(Mesh);
	}

	const std::vector<std::shared_ptr<graphics::CMesh>>& C3DObject::GetMeshList() const
	{
		return m_MeshList;
	}

	void C3DObject::AddMaterial(const std::shared_ptr<graphics::CMaterial>& Material)
	{
		m_MaterialList.push_back(Material);
	}

	void C3DObject::AddAnimationSkin(const std::shared_ptr<animation::CSkin >& Skin)
	{
		m_AnimationSkinList.push_back(Skin);
	}

	void C3DObject::AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& Clip)
	{
		m_AnimationClipList.push_back(Clip);
	}

	const std::vector<std::shared_ptr<graphics::CMaterial>>& C3DObject::GetMaterialList() const
	{
		return m_MaterialList;
	}

	void C3DObject::SetRootNodeIndexList(const std::vector<std::vector<int>>& RootNodeIndexList)
	{
		m_RootNodeIndexList = RootNodeIndexList;
	}

	const std::vector<std::vector<int>>& C3DObject::GetRootNodeIndexList() const
	{
		return m_RootNodeIndexList;
	}

	const glm::vec3& C3DObject::GetPos() const
	{
		return m_ObjectTransform->GetPos();
	}

	void C3DObject::SetPos(const glm::vec3& Pos)
	{
		m_ObjectTransform->SetPos(Pos);
	}

	const glm::quat& C3DObject::GetRot() const
	{
		return m_ObjectTransform->GetRot();
	}

	void C3DObject::SetRot(const glm::quat& Rot)
	{
		m_ObjectTransform->SetRot(Rot);
	}

	const glm::vec3& C3DObject::GetScale() const
	{
		return m_ObjectTransform->GetScale();
	}

	void C3DObject::SetScale(const glm::vec3& Scale)
	{
		m_ObjectTransform->SetScale(Scale);
	}

	void C3DObject::SetPlayClipIndex(int Index)
	{
		m_CurrentClipIndex = Index;
	}

	bool  C3DObject::IsPlayingAnimation()
	{
		return (m_CurrentClipIndex >= 0 && m_CurrentClipIndex < m_AnimationClipList.size());
	}
}