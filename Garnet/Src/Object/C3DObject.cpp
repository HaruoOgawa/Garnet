#include "C3DObject.h"
#include "../Format/CPathFormatter.h"
#include "../GLTF/CGLTFImporter.h"
#include "../LoadWorker/CLoadWorker.h"

#if defined(USE_FBX)

#ifdef USE_SMALL_FBX

#include "../FBX/CSmallFBXImporter.h"
#else
#include "../FBX/CFBXImporter.h"
#endif // USE_SMALL_FBX

#endif

#ifdef USE_MMD
#include "../MMD/PMX/CPmxImporter.h"
#endif

namespace object
{
	C3DObject::C3DObject(const std::string& PassName, const std::string& DepthPassName):
		m_IsCreated(false),
		m_ExistFirstDelayResource(false),
		m_PassName(PassName),
		m_DepthPassName(DepthPassName),
		m_ObjectTransform(std::make_shared<math::CTransform>()),
#ifdef USE_ANIMATION
		m_AnimationController(std::make_shared<animation::CAnimationController>()),
		m_BlendShapeController(std::make_shared<animation::CBlendShapeController>()),
#endif
		m_TextureSet(std::make_shared<graphics::CTextureSet>()),
		m_FileName(""),
		m_ObjectType(E3DObjectType::None),
		m_DepthMF(nullptr)
	{
	}

	C3DObject::~C3DObject()
	{
		m_IsCreated = false;
		m_NodeList.clear();
		m_MaterialList.clear();
	}

	void C3DObject::SetBinaryData(const std::vector<unsigned char>& Data, const std::string& FileName, E3DObjectType ObjectType)
	{
		m_BinaryData = Data;
		m_FileName = FileName;
		m_ObjectType = ObjectType;
	}

	bool C3DObject::CreateSimply(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine,
		const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo,
		const std::shared_ptr<graphics::CMaterial>& Material, const std::shared_ptr<graphics::CMaterialFrame>& DepthMF, 
		const std::shared_ptr<math::CTransform> NodeTransform, const std::shared_ptr<physics::IPhysicsObject>& PhysicsObject)
	{
		// Material
		AddMaterial(Material);

		// Mesh
		std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
		Mesh->CreateSimpleMesh(createInfo, 0);

		AddMesh(Mesh);

		// Node
		std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, 0);
		Node->SetLocalTransform(NodeTransform);
		Node->SetPhysicsObject(PhysicsObject);
		AddNode(Node);

		// Create
		if (!Create(pGraphicsAPI, pPhysicsEngine, DepthMF)) return false;

		return true;
	}

	bool C3DObject::CreateFromMemory(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CMaterialFrame>& BaseMF, const std::shared_ptr<graphics::CMaterialFrame>& DepthMF)
	{
		m_DepthMF = DepthMF;

		if (m_BinaryData.empty()) return false;

		switch (m_ObjectType)
		{
#ifdef USE_GLTF
		case object::E3DObjectType::glTF:
			{
				std::string BaseDir = format::CPathFormatter::GetParentDir(m_FileName);
				if (!gltf::CGLTFImporter::ImportFromString(pGraphicsAPI, m_BinaryData, BaseDir, this, BaseMF)) return false;
			}
			break;
			
		case object::E3DObjectType::glb:
			if (!gltf::CGLTFImporter::ImportFromMemory(pGraphicsAPI, m_BinaryData, this, BaseMF)) return false;
			break;
#endif
#if defined(USE_FBX)
		case object::E3DObjectType::Fbx:
#ifdef USE_SMALL_FBX
			if (!fbx::CSmallFBXImporter::ImportFBX(pGraphicsAPI, m_BinaryData, this, BaseMF)) return false;
#else
			if (!fbx::CFBXImporter::ImportFBX(pGraphicsAPI, m_FileName, this, BaseMF)) return false;
#endif // USE_SMALL_FBX
			break;
#endif
		case object::E3DObjectType::Pmx:
#ifdef USE_MMD
			if (!mmd::CPmxImporter::ImportPmx(pGraphicsAPI, pPhysicsEngine, pLoadWorker, m_FileName, m_BinaryData, this, BaseMF)) return false;
#endif
			break;
		default:
			break;
		}

		m_BinaryData.clear();

		// インポートの結果、遅延ロードリソースが見つかった時はCreateを後回しにする
		if (m_RuntimeLoadResourceList.size() != 0)
		{
			m_ExistFirstDelayResource = true;

			return true;
		}

		if (!Create(pGraphicsAPI, pPhysicsEngine, DepthMF)) return false;

		return true;
	}

	bool C3DObject::Create(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, const std::shared_ptr<graphics::CMaterialFrame>& DepthMF)
	{
		// DefaultLocalTransformを保存する
		ApplyDefaultLocalTransform();

		// 親ノードを設定
		ApplyParentNode();

		// ワールド行列の計算
		CalcWorldMatrix();

		// 物理
		CreatePhysics(pPhysicsEngine);
		ApplyPhysicsConstraint(pPhysicsEngine);

		// Material
		for (auto& Material : m_MaterialList)
		{
			if (!Material->Create(m_TextureSet)) return false;
			
			if (DepthMF)
			{
				if (!Material->CreateDepthMaterial(pGraphicsAPI, DepthMF)) return false;
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

		m_IsCreated = true;

		return true;
	}

	void C3DObject::ApplyDefaultLocalTransform()
	{
		for (const auto& Node : m_NodeList)
		{
			Node->SaveAsDefaultLocalTransform();
		}
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

	// 物理
	void C3DObject::CreatePhysics(physics::IPhysicsEngine* pPhysicsEngine)
	{
		for (const auto& Node : m_NodeList)
		{
			Node->CreatePhysicsObject(pPhysicsEngine);
		}
	}

	void C3DObject::ApplyPhysicsConstraint(physics::IPhysicsEngine* pPhysicsEngine)
	{
		for (const auto& Node : m_NodeList)
		{
			Node->ApplyPhysicsConstraint(pPhysicsEngine);
		}
	}

	void C3DObject::ApplyPhysicsWorldMatrix()
	{
		for (const auto& Node : m_NodeList)
		{
			Node->ApplyPhysicsWorldMatrix();
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

	bool C3DObject::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, float DeltaSecondsTime)
	{
		for (auto& Resource : m_RuntimeLoadResourceList)
		{
			switch (Resource->GetStatus())
			{
			case resource::ELoadStatus::Loaded:
			{
				m_RuntimeLoadResourceList.erase(m_RuntimeLoadResourceList.begin());
				m_RuntimeLoadResourceList.shrink_to_fit();

				return true;
			}

			case resource::ELoadStatus::None:
			case resource::ELoadStatus::Loading:
			default:
				break;
			}
		}

		// Create関数を伴う初回動的ロード
		if (m_ExistFirstDelayResource && m_RuntimeLoadResourceList.size() == 0 && !m_IsCreated)
		{
			if (!Create(pGraphicsAPI, pPhysicsEngine, m_DepthMF)) return false;

			m_ExistFirstDelayResource = false;
		}

		if (!m_IsCreated) return true;

		// マテリアルの参照カウントをリセット
		for (auto& Material : m_MaterialList)
		{
			//
			if (!Material) continue;
			Material->ResetDynamicOffset();

			//
			auto DepthMaterial = Material->GetDepthMaterial();

			if (!DepthMaterial) continue;
			DepthMaterial->ResetDynamicOffset();
		}

#ifdef USE_ANIMATION
		if (!m_AnimationController->Update(DeltaSecondsTime)) return false;
		if (!m_BlendShapeController->Update(DeltaSecondsTime, m_NodeList)) return false;
#endif
		// ワールド行列の更新
		// 全ノードマイフレーム更新しているので、そのうちキャッシュを入れて更新は必要なものだけにする
		CalcWorldMatrix();

#ifdef USE_ANIMATION
		// IKの計算を行う
		if (!m_AnimationController->CalculateIK(m_NodeList)) return false;

		// 付与ボーンの位置を再計算
		if (!m_AnimationController->ReCalculateGrantBone(m_NodeList)) return false;
#endif

		// 物理演算の結果を反映する
		ApplyPhysicsWorldMatrix();

#ifdef USE_ANIMATION
		// IKや物理演算が終わって最終的なWorldMatrixが確定した段階でSkinMatrixを計算する
		m_CurrentSkinMatrixList.clear();
		if (!m_AnimationController->CalCSkinMatrixList(m_CurrentSkinMatrixList, m_ObjectTransform->GetModelMatrix())) return false;
#endif

		return true;
	}

	bool C3DObject::Draw(bool IsDepthPass, bool DrawOutline, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo,
		const std::shared_ptr<object::C3DObject>& DebugSphere)
	{
		if (!m_IsCreated) return true;

		// 描画
		for (const auto& Node : m_NodeList)
		{
			int MeshIndex = Node->GetMeshIndex();
			if (MeshIndex < 0 || MeshIndex >= m_MeshList.size()) continue;

			const auto& WorldMatrix = m_ObjectTransform->GetModelMatrix() * Node->GetWorldMatrix();
			const auto& Mesh = m_MeshList[MeshIndex];

			int SkeletonIndex = Node->GetSkeletonIndex();

			// モーフウェイト
			const auto& MorphWeights = Node->GetCurrentMorphWeights();

			for (int PrimitiveIndex = 0; PrimitiveIndex < Mesh->GetPrimitiveList().size(); PrimitiveIndex++)
			{
				const auto& Primitive = Mesh->GetPrimitiveList()[PrimitiveIndex];

				int MaterialIndex = Primitive->GetMaterialIndex();
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
				
				// マテリアルの参照カウントをダイナミックオフセットとして使用する
				int DynamicOffsetNum = Material->GetDynamicOffset();

				// ダイナミックオフセットがマテリアル参照数よりも大きい時は終了する
				if (DynamicOffsetNum > Material->GetRefCount()) continue;

				// アウトライン
				if (DrawOutline)
				{
					if (DrawOutline != Material->IsDrawOutline()) continue;

					Material->SetCullMode(graphics::ECullMode::CULL_FRONT);
				}

				// 共通のユニフォームバッファの更新
				glm::mat4 lightVPMat = DrawInfo->GetLightProjection()->GetPrejectionMatrix() * DrawInfo->GetLightCamera()->GetViewMatrix();

				Material->SetUniformValue("drawPathIndex", &DynamicOffsetNum, sizeof(int), DynamicOffsetNum);
				Material->SetUniformValue("model", &WorldMatrix[0][0], sizeof(glm::mat4), DynamicOffsetNum);
				Material->SetUniformValue("view", &Camera->GetViewMatrix()[0][0], sizeof(glm::mat4), DynamicOffsetNum);
				Material->SetUniformValue("proj", &Projection->GetPrejectionMatrix()[0][0], sizeof(glm::mat4), DynamicOffsetNum);
				Material->SetUniformValue("lightVPMat", &lightVPMat[0][0], sizeof(glm::mat4), DynamicOffsetNum);
				Material->SetUniformValue("lightDir", &DrawInfo->GetLightCamera()->GetViewDir()[0], sizeof(glm::vec3), DynamicOffsetNum);
				Material->SetUniformValue("lightColor", &DrawInfo->GetLightColor()[0], sizeof(glm::vec4), DynamicOffsetNum);
				Material->SetUniformValue("cameraPos", &Camera->GetPos()[0], sizeof(glm::vec3), DynamicOffsetNum);
				Material->SetUniformValue("time", &glm::vec1(DrawInfo->GetSecondsTime())[0], sizeof(float), DynamicOffsetNum);
				Material->SetUniformValue("deltaTime", &glm::vec1(DrawInfo->GetDeltaSecondsTime())[0], sizeof(float), DynamicOffsetNum);
#ifdef USE_ANIMATION
				Material->SetUniformValue("useSkinMeshAnimation", &glm::ivec1((m_AnimationController->IsEnabledSkeleton() ? 1 : 0))[0], sizeof(glm::ivec1), DynamicOffsetNum);

				// SkinMatrixをShaderに渡す
				if (m_CurrentSkinMatrixList.size() > 0)
				{
					Material->SetUniformValue("r_SkinMatrixBuffer", &m_CurrentSkinMatrixList[0], sizeof(glm::mat4) * static_cast<int>(m_CurrentSkinMatrixList.size()), DynamicOffsetNum);
				}
#endif
				// モーフ
				for (int MorphIndex = 0; MorphIndex < MorphWeights.size(); MorphIndex++)
				{
					std::string MorphUniformName = "MorphWeight_" + std::to_string(MorphIndex);
					float Weight = MorphWeights[MorphIndex];

					Material->SetUniformValue(MorphUniformName, &glm::vec1(Weight)[0], sizeof(float), DynamicOffsetNum);
				}

				// 描画実行
				if (!Primitive->Draw(Material, DynamicOffsetNum, IsDepthPass)) return false;

				// マテリアルの参照カウントをインクリメントする
				Material->IncreaseDynamicOffset();

				// 描画準備のために変更した設定を元に戻す
				Material->ResetToDefaultCullMode();
			}
		}
		
#ifdef USE_ANIMATION
		/*
		if(DebugSphere)
		{
			const auto& Skeleton = m_AnimationController->GetSkeleton();
			if(Skeleton)
			{
				for (const auto& Bone : Skeleton->GetBoneList())
				{
					//if (Bone->GetBoneName() == animation::EHumanoidBones::None) continue;

					const auto& BoneNode = Bone->GetBoneNode();

					// Debug用: Boneの描画
					{
						DebugSphere->SetPos(m_ObjectTransform->GetModelMatrix() * BoneNode->GetWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
						DebugSphere->SetScale(glm::vec3(0.025f));
					}
					
					// Debug用: ローカル軸の描画(SphereをBoxに変更する)
					{
						//DebugSphere->SetScale(glm::vec3(0.025f, 0.025f, 0.025f * 4.0f));
						//DebugSphere->SetRot(BoneNode->GetDefaultLocalTransform()->GetRot());
						//DebugSphere->SetPos(m_ObjectTransform->GetModelMatrix()* BoneNode->GetWorldMatrix()* glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
					}

					DebugSphere->GetMaterialList()[0]->SetUniformValue("useColor", &glm::ivec1(1)[0], sizeof(glm::ivec1));
					
					if(BoneNode->GetPhysicsObject() && BoneNode->GetPhysicsObject()->IsStatic())
					{
						DebugSphere->GetMaterialList()[0]->SetUniformValue("baseColor", &glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)[0], sizeof(glm::vec4));
					}
					else
					{
						DebugSphere->GetMaterialList()[0]->SetUniformValue("baseColor", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0], sizeof(glm::vec4));
					}

					{
						glm::mat4 Matrix = m_ObjectTransform->GetModelMatrix() * BoneNode->GetWorldMatrix();

						glm::vec3 WorldPos = glm::vec3(0.0f);
						glm::quat WorldRotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
						glm::vec3 WorldScale = glm::vec3(1.0f);

						math::CTransform::CastModelMatrixToTransform(Matrix, WorldPos, WorldRotate, WorldScale);

						DebugSphere->SetPos(WorldPos);
						DebugSphere->SetRot(WorldRotate);
						DebugSphere->SetScale(WorldScale);
					}

					if (!DebugSphere->Draw(IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
				}
			}
		}
		*/
#endif

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

#ifdef USE_ANIMATION
	void C3DObject::SetAnimationSkeleton(const std::shared_ptr<animation::CSkeleton >& Skeleton)
	{
		m_AnimationController->SetAnimationSkeleton(Skeleton);
	}

	void C3DObject::AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& Clip)
	{
		m_AnimationController->AddAnimationClip(Clip);
	}

	void C3DObject::AddHumanoidAnimationClip(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::string& MotionName, animation::SAnimationLayout Layout, bool IsLoop)
	{
		m_AnimationController->AddHumanoidAnimationClip(SourceClip, MotionName, Layout, IsLoop);
	}

	void C3DObject::AddBlendShapeClip(const std::shared_ptr<animation::CBlendShapeClip>& Clip)
	{
		m_BlendShapeController->AddBlendShapeClip(Clip);
	}

	const std::vector<std::shared_ptr<animation::CAnimationClip>>& C3DObject::GetAnimationClipList() const
	{
		return m_AnimationController->GetAnimationClipList();
	}
#endif

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

	void C3DObject::ChangeMotion(int Index)
	{
		m_AnimationController->ChangeMotion(Index);
	}

	void C3DObject::ChangeMotion(const std::string& MotionName)
	{
		m_AnimationController->ChangeMotion(MotionName);
	}

	void C3DObject::ChangeBlendShape(int Index)
	{
		m_BlendShapeController->ChangeBlendShape(Index, m_NodeList);
	}

	const std::shared_ptr<graphics::CTextureSet>& C3DObject::GetTextureSet() const
	{
		return m_TextureSet;
	}

	void C3DObject::AddRuntimeLoadResource(const std::shared_ptr <resource::IResource>& Resource)
	{
		m_RuntimeLoadResourceList.push_back(Resource);
	}
}