#include "C3DObject.h"
#include "../Scriptable/CComponent.h"

namespace object
{
	C3DObject::C3DObject():
		m_IsCreated(false),
		m_FileName(std::string()),
		m_ObjectName("3DObject"),
		m_ObjectTransform(std::make_shared<math::CTransform>()),
		m_Enabled(true),
#ifdef USE_ANIMATION
		m_AnimationController(std::make_shared<animation::CAnimationController>()),
		m_BlendShapeController(std::make_shared<animation::CBlendShapeController>()),
#endif
		m_MorphController(std::make_shared<graphics::CMorphController>()),
		m_TextureSet(std::make_shared<graphics::CTextureSet>()),
		m_DepthMF(nullptr)
	{
	}

	C3DObject::~C3DObject()
	{
		m_IsCreated = false;
		m_NodeList.clear();
	}

	void C3DObject::Reset()
	{
#ifdef USE_ANIMATION
		if (m_AnimationController)
		{
			const auto& CurrentClip = m_AnimationController->GetCurrentClip();

			if (CurrentClip)
			{
				CurrentClip->Reset(m_AnimationController->GetSkeleton());
			}
		}
		
		if (m_BlendShapeController)
		{
			for (const auto& ClipPair : m_BlendShapeController->GetBlendShapeClipMap())
			{
				ClipPair.second->Reset(nullptr);
			}
		}
#endif

		for (const auto& Component : m_ComponentList)
		{
			Component->Reset();
		}

		for (auto& Node : m_NodeList)
		{
			for (const auto& Component : Node->GetComponentList())
			{
				Component->Reset();
			}
		}
	}

	void C3DObject::SetFileName(const std::string& Name)
	{
		m_FileName = Name;
	}

	const std::string& C3DObject::GetFileName() const
	{
		return m_FileName;
	}

	void C3DObject::AddDefaultMaterialFrame(const std::string& Name)
	{
		m_DefaultMaterialFrameList.push_back(Name);
	}

	const std::vector<std::string>& C3DObject::GetDefaultMaterialFrameList() const
	{
		return m_DefaultMaterialFrameList;
	}

	void C3DObject::SetObjectName(const std::string& Name)
	{
		m_ObjectName = Name;
	}
	const std::string& C3DObject::GetObjectName() const
	{
		return m_ObjectName;
	}

	const std::vector<std::string>& C3DObject::GetPassNameList() const
	{
		return m_PassNameList;
	}

	void C3DObject::AddPassName(const std::string& Name)
	{
		m_PassNameList.push_back(Name);
	}

	// コンポーネント
	void C3DObject::AddComponent(const std::shared_ptr<scriptable::CComponent>& Component)
	{
		m_ComponentList.push_back(Component);
	}

	const std::vector<std::shared_ptr<scriptable::CComponent>>& C3DObject::GetComponentList() const
	{
		return m_ComponentList;
	}

	bool C3DObject::HasTLTrackContent() const
	{
		return (!m_TLNodeList.empty() || !m_TLMaterial.empty());
	}

	void C3DObject::AddTLNode(const std::shared_ptr<CNode>& Node)
	{
		m_TLNodeList.emplace(Node);
	}

	const std::set<std::shared_ptr<CNode>>& C3DObject::GetTLNodeList() const
	{
		return m_TLNodeList;
	}

	void C3DObject::AddTLMaterial(const std::shared_ptr<graphics::CMaterial>& Material)
	{
		m_TLMaterial.emplace(Material);
	}

	const std::set<std::shared_ptr<graphics::CMaterial>>& C3DObject::GetTLMaterial() const
	{
		return m_TLMaterial;
	}

	const std::shared_ptr<math::CTransform>& C3DObject::GetObjectTransform() const
	{
		return m_ObjectTransform;
	}

	void C3DObject::SetEnabled(bool Flag)
	{
		m_Enabled = Flag;
	}

	bool C3DObject::IsEnabled() const
	{
		return m_Enabled;
	}
	
	bool C3DObject::AddPresetSimply(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine,
		const std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>>& createInfo,
		graphics::EPresetPrimitiveType PresetType,
		const std::shared_ptr<graphics::CMaterial>& Material, 
		const std::shared_ptr<math::CTransform> NodeTransform, const std::shared_ptr<physics::IPhysicsObject>& PhysicsObject)
	{
		int MeshIndex = static_cast<int>(m_MeshList.size());
		int NodeIndex = static_cast<int>(m_NodeList.size());

		// Mesh
		std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
		Mesh->CreatePresetSimpleMesh(pGraphicsAPI, createInfo.first, createInfo.second, Material, PresetType);

		AddMesh(Mesh);

		// Node
		std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(MeshIndex, NodeIndex);
		Node->SetLocalTransform(NodeTransform);
		Node->AddPhysicsObject(PhysicsObject);
		AddNode(Node);

		return true;
	}
	
	bool C3DObject::CreatePresetSimply(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine,
		const std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>>& createInfo,
		graphics::EPresetPrimitiveType PresetType,
		const std::shared_ptr<graphics::CMaterial>& Material, 
		const std::shared_ptr<math::CTransform> NodeTransform, const std::shared_ptr<physics::IPhysicsObject>& PhysicsObject)
	{
		if (!AddPresetSimply(pGraphicsAPI, pPhysicsEngine, createInfo, PresetType, Material, NodeTransform, PhysicsObject)) return false;

		// Create
		if (!Create(pGraphicsAPI, pPhysicsEngine)) return false;

		return true;
	}

	bool C3DObject::Create(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine)
	{
		// タイムライントラックの参照リストを追加
		AsignTrackRef();

		// 既に作られていたらスキップ
		if (m_IsCreated) return true;

		// DefaultLocalTransformを保存する
		ApplyDefaultLocalTransform();

		// 親ノードを設定
		ApplyParentNode();

		// ワールド行列の計算
		CalcWorldMatrix();

		// 物理
		CreatePhysics(pPhysicsEngine);
		ApplyPhysicsConstraint(pPhysicsEngine);

		// Primitive
		bool ExistMorph = false;

		for (const auto& Mesh : m_MeshList)
		{
			if (!Mesh->Create(m_TextureSet, m_PassNameList)) return false;

			// モーフ処理が必要かどうか
			if (Mesh->GetMorphDataList().size() > 0)
			{
				ExistMorph = true;
			}
		}

		// モーフ
		if (ExistMorph)
		{
			if (!m_MorphController->Create(m_MeshList)) return false;
		}

		m_IsCreated = true;

		return true;
	}

	void C3DObject::AsignTrackRef()
	{
		// TrackID参照リスト
		// Node
		for (const auto& Node : m_NodeList)
		{
			if (!Node->GetRefTrackIDList().empty())
			{
				m_TLNodeList.emplace(Node);
			}
		}

		// Material
		for (const auto& Mesh : m_MeshList)
		{
			for (const auto& Primitive : Mesh->GetPrimitiveList())
			{
				for (const auto& Renderer : Primitive->GetRendererList())
				{
					const auto& Material = std::get<1>(Renderer);
					if (!Material) continue;

					if (!Material->GetRefTrackIDList().empty())
					{
						m_TLMaterial.emplace(Material);
					}
				}
			}
		}
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
		for (const int RootNodeIndex : m_RootNodeIndexList)
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

	void C3DObject::AlignPhysicsJoint()
	{
		for (const auto& Node : m_NodeList)
		{
			Node->AlignPhysicsJoint();
		}
	}

	// ワールド行列の初期値を計算(アニメーション等で後々更新される可能性がある)
	void C3DObject::CalcWorldMatrix()
	{
		if (!m_RootNodeIndexList.empty()) 
		{
			// ルートノードから順に走破してワールド行列を計算する
			for (const int RootNodeIndex : m_RootNodeIndexList)
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

	glm::mat4 C3DObject::CalcNoScaledWorldMatrix(const std::shared_ptr<object::CNode>& ChildNode)
	{
		glm::mat4 result = glm::mat4(1.0f);

		auto Target = ChildNode;

		for (;;)
		{
			if (!Target) break;

			result = glm::translate(glm::mat4(1.0f), Target->GetPos()) * glm::toMat4(Target->GetRot()) * result;

			Target = Target->GetParentNode();
		}

		return result;
	}

	bool C3DObject::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, float DeltaSecondsTime, resource::CLoadWorker* pLoadWorker,
		const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_IsCreated) return true;

#ifdef USE_ANIMATION
		if (!m_AnimationController->Update(DeltaSecondsTime)) return false;
		if (!m_BlendShapeController->Update(DeltaSecondsTime)) return false;
#endif

		// ワールド行列の更新
		// ボーンアニメーションよりも後、IKよりも前に計算
		CalcWorldMatrix();

#ifdef USE_ANIMATION
		// IKの計算を行う
		if (!m_AnimationController->CalculateIK(m_NodeList)) return false;

		// 付与ボーンの位置を計算
		if (!m_AnimationController->CalculateGrantBone(m_NodeList)) return false;

		// モーフ
		if (!m_MorphController->Update(DeltaSecondsTime, m_MeshList)) return false;
#endif
		// 物理ジョイントの位置をボーン位置に合わせる
		//AlignPhysicsJoint();

		// コンポーネント
		for (const auto& Component : m_ComponentList)
		{
			if (!Component->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState, shared_from_this(), nullptr)) return false;
		}
		
		for (const auto& Node : GetNodeList())
		{
			for (const auto& Component : Node->GetComponentList())
			{
				if (!Component->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState, shared_from_this(), Node)) return false;
			}
		}

		return true;
	}

	bool C3DObject::LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, float DeltaSecondsTime)
	{
		return true;
	}

	bool C3DObject::FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, float DeltaSecondsTime)
	{
		// 物理演算の結果を反映する
		ApplyPhysicsWorldMatrix();

#ifdef USE_ANIMATION
		// IKや物理演算が終わって最終的なWorldMatrixが確定した段階でSkinMatrixを計算する
		m_CurrentSkinMatrixList.clear();
		if (!m_AnimationController->CalCSkinMatrixList(m_CurrentSkinMatrixList, m_ObjectTransform->GetModelMatrix())) return false;
#endif

		return true;
	}

	bool C3DObject::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo,
		const std::shared_ptr<object::C3DObject>& DebugSphere)
	{
		if (!m_IsCreated) return true;

		if (!m_Enabled) return true;

		// 描画パスが違うなら描画しない
		auto it = std::find(m_PassNameList.begin(), m_PassNameList.end(), pGraphicsAPI->GetCurrentRenderPassName());
		if (it == m_PassNameList.end()) return true;

		for (const auto& Component : m_ComponentList)
		{
			if (!Component->Draw(pGraphicsAPI, Camera, Projection, DrawInfo, shared_from_this(), nullptr)) return false;
		}

		// 描画
		if (!m_RootNodeIndexList.empty())
		{
			// ルートノードから順に子要素を探査して描画
			for (const int RootNodeIndex : m_RootNodeIndexList)
			{
				if (RootNodeIndex < 0 || RootNodeIndex >= m_NodeList.size()) continue;

				auto& RootNode = m_NodeList[RootNodeIndex];

				if (!DrawFromNode(RootNode, pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			}
		}
		else
		{
			// ルートノードが指定されていないので全ノードを順番に描画
			for (const auto& Node : m_NodeList)
			{
				if (!Draw(Node, pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
			}
		}

		//if (!DrawDebugBone(pGraphicsAPI , Camera, Projection, DrawInfo, DebugSphere)) return false;
		//if (!DrawDebugPhysics(pGraphicsAPI, Camera, Projection, DrawInfo, DebugSphere)) return false;

		return true;
	}

	bool C3DObject::DrawFromNode(const std::shared_ptr<CNode>& Node, api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera,
		const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		// 非表示だったら子要素も描画しない
		if (!Node->IsEnabled()) return true;

		// 自分自身の描画
		if (!Draw(Node, pGraphicsAPI, Camera, Projection, DrawInfo)) return false;

		// 子要素の描画
		for (const int ChildIndex : Node->GetChildrenNodeIndexList())
		{
			if (ChildIndex < 0 || ChildIndex >= m_NodeList.size()) continue;

			auto& ChildNode = m_NodeList[ChildIndex];
			if (!DrawFromNode(ChildNode, pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	bool C3DObject::Draw(const std::shared_ptr<CNode>& Node, api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera,
		const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		for (const auto& Component : Node->GetComponentList())
		{
			if (!Component->Draw(pGraphicsAPI, Camera, Projection, DrawInfo, shared_from_this(), Node)) return false;
		}

		int MeshIndex = Node->GetMeshIndex();
		if (MeshIndex < 0 || MeshIndex >= m_MeshList.size()) return true;

		const auto& WorldMatrix = m_ObjectTransform->GetModelMatrix() * Node->GetWorldMatrix();
		const auto& Mesh = m_MeshList[MeshIndex];

		int SkeletonIndex = Node->GetSkeletonIndex();

		float MipCount = 1.0f;
		const auto& CubeTexList = m_TextureSet->GetCubeMapList();
		const auto& Specular_Tex = m_TextureSet->GetSpecular_Tex();
		if (CubeTexList.size() > 0)
		{
			MipCount = CubeTexList[0]->GetMipCount();
		}
		else if (Specular_Tex)
		{
			MipCount = Specular_Tex->GetMipCount();
		}

		for (int PrimitiveIndex = 0; PrimitiveIndex < Mesh->GetPrimitiveList().size(); PrimitiveIndex++)
		{
			const auto& Primitive = Mesh->GetPrimitiveList()[PrimitiveIndex];

			// Materialの標準パラメーターを設定
			for (const auto& Renderer : Primitive->GetRendererList())
			{
				const auto& Material = std::get<1>(Renderer);

				if (!Material) return true;

				// 共通のユニフォームバッファの更新
				glm::mat4 lightVMat = DrawInfo->GetLightCamera()->GetViewMatrix();
				glm::mat4 lightPMat = DrawInfo->GetLightProjection()->GetPrejectionMatrix();
				glm::mat4 lightVPMat = lightPMat * lightVMat;

				Material->SetUniformValue("model", &WorldMatrix[0][0], sizeof(glm::mat4));
				Material->SetUniformValue("view", &Camera->GetViewMatrix()[0][0], sizeof(glm::mat4));
				Material->SetUniformValue("proj", &Projection->GetPrejectionMatrix()[0][0], sizeof(glm::mat4));
				Material->SetUniformValue("lightVMat", &lightVMat[0][0], sizeof(glm::mat4));
				Material->SetUniformValue("lightPMat", &lightPMat[0][0], sizeof(glm::mat4));
				Material->SetUniformValue("lightVPMat", &lightVPMat[0][0], sizeof(glm::mat4));
				glm::vec3 lightDir = DrawInfo->GetLightCamera()->GetViewDir();
				Material->SetUniformValue("lightDir", &glm::vec4(lightDir.x, lightDir.y, lightDir.z, 0.0f)[0], sizeof(glm::vec4));
				glm::vec3 lightPos = DrawInfo->GetLightCamera()->GetPos();
				Material->SetUniformValue("lightPos", &glm::vec4(lightPos.x, lightPos.y, lightPos.z, 0.0f)[0], sizeof(glm::vec4));
				Material->SetUniformValue("lightColor", &DrawInfo->GetLightColor()[0], sizeof(glm::vec4));
				glm::vec3 CameraPos = Camera->GetPos();
				Material->SetUniformValue("cameraPos", &glm::vec4(CameraPos.x, CameraPos.y, CameraPos.z, 1.0f)[0], sizeof(glm::vec4));
				Material->SetUniformValue("time", &glm::vec1(DrawInfo->GetSecondsTime())[0], sizeof(float));
				Material->SetUniformValue("deltaTime", &glm::vec1(DrawInfo->GetDeltaSecondsTime())[0], sizeof(float));
				Material->SetUniformValue("resolution", &Projection->GetScreenResolution()[0], sizeof(glm::vec2));

				Material->SetUniformValue("useSpatialCulling", &glm::ivec1(DrawInfo->IsSpatialCulling() ? 1 : 0)[0], sizeof(int));
				Material->SetUniformValue("spatialCullPos", &DrawInfo->GetSpatialCullPos()[0], sizeof(float) * 4);
#ifdef USE_ANIMATION
				Material->SetUniformValue("useSkinMeshAnimation", &glm::ivec1((m_AnimationController->IsEnabledSkeleton() ? 1 : 0))[0], sizeof(glm::ivec1));

				// SkinMatrixをShaderに渡す
				if (m_CurrentSkinMatrixList.size() > 0)
				{
					Material->SetUniformValue("r_SkinMatrixBuffer", &m_CurrentSkinMatrixList[0], sizeof(glm::mat4) * static_cast<int>(m_CurrentSkinMatrixList.size()));
				}
#endif
				Material->SetUniformValue("mipCount", &glm::vec1(MipCount)[0], sizeof(float));

			}
			
			// 描画実行
			if (!Primitive->Draw(pGraphicsAPI, WorldMatrix, Camera, Projection)) return false;
		}

		return true;
	}

	bool C3DObject::DrawDebugBone(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& DebugSphere)
	{
#ifdef USE_ANIMATION
		if(DebugSphere)
		{
			const auto& Skeleton = m_AnimationController->GetSkeleton();
			if(Skeleton)
			{
				for (const auto& Bone : Skeleton->GetBoneList())
				{
					//if (Bone->GetBoneName() == animation::EHumanoidBones::None) continue;

					const auto& BoneNode = std::get<1>(Bone)->GetBoneNode();

					// Debug用: Boneの描画
					{
						DebugSphere->SetPos(m_ObjectTransform->GetModelMatrix() * BoneNode->GetWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
						DebugSphere->SetScale(glm::vec3(0.05f));
					}

					// Debug用: ローカル軸の描画(SphereをBoxに変更する)
					{
						//DebugSphere->SetScale(glm::vec3(0.025f, 0.025f, 0.025f * 4.0f));
						//DebugSphere->SetRot(BoneNode->GetDefaultLocalTransform()->GetRot());
						//DebugSphere->SetPos(m_ObjectTransform->GetModelMatrix()* BoneNode->GetWorldMatrix()* glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
					}

					for (const auto& Mesh : DebugSphere->GetMeshList())
					{
						for (const auto& Primitive : Mesh->GetPrimitiveList())
						{
							for (const auto& Renderer : Primitive->GetRendererList())
							{
								const auto& Material = std::get<1>(Renderer);
								if (!Material) continue;

								Material->SetUniformValue("useColor", &glm::ivec1(1)[0], sizeof(glm::ivec1));
								Material->SetUniformValue("baseColor", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0], sizeof(glm::vec4));
								Material->SetUniformValue("baseColorFactor", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0], sizeof(glm::vec4));
							}
						}
					}
					
					if (!DebugSphere->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
				}
			}
		}
#endif
		return true;
	}

	bool C3DObject::DrawDebugPhysics(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& DebugSphere)
	{
#ifdef USE_ANIMATION
		if(DebugSphere)
		{
			const auto& Skeleton = m_AnimationController->GetSkeleton();
			if(Skeleton)
			{
				for (const auto& BonePair : Skeleton->GetBoneList())
				{
					const auto& Bone = std::get<1>(BonePair);

					if (Bone->GetBoneName() == animation::EHumanoidBones::Center) continue;

					const auto& BoneNode = Bone->GetBoneNode();
					
					for (const auto& PhysicsObject : BoneNode->GetPhysicsObjectList())
					{
						for (const auto& Mesh : DebugSphere->GetMeshList())
						{
							for (const auto& Primitive : Mesh->GetPrimitiveList())
							{
								for (const auto& Renderer : Primitive->GetRendererList())
								{
									const auto& Material = std::get<1>(Renderer);
									if (!Material) continue;

									Material->SetUniformValue("useColor", &glm::ivec1(1)[0], sizeof(glm::ivec1));

									if (PhysicsObject->IsStatic())
									{
										Material->SetUniformValue("baseColor", &glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)[0], sizeof(glm::vec4));
									}
									else
									{
										Material->SetUniformValue("baseColor", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0], sizeof(glm::vec4));
									}
								}
							}
						}

						{
							glm::mat4 Matrix = m_ObjectTransform->GetModelMatrix() * PhysicsObject->GetCurrentPhysicsWorldMatrix();

							glm::vec3 WorldPos = glm::vec3(0.0f);
							glm::quat WorldRotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
							glm::vec3 WorldScale = glm::vec3(1.0f);

							math::CTransform::CastModelMatrixToTransform(Matrix, WorldPos, WorldRotate, WorldScale);

							WorldScale = m_ObjectTransform->GetScale() * PhysicsObject->GetSize();

							DebugSphere->SetPos(WorldPos);
							DebugSphere->SetRot(WorldRotate);
							DebugSphere->SetScale(WorldScale);
						}

						if (!DebugSphere->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
					}
				}
			}
		}
#endif
		return true;
	}

	void C3DObject::AddNode(const std::shared_ptr<CNode>& Node)
	{
		m_NodeList.push_back(Node);
	}

	void C3DObject::ReserveNodeCount(size_t Count)
	{
		m_NodeList.reserve(Count);
	}

	const std::vector<std::shared_ptr<CNode>>& C3DObject::GetNodeList() const
	{
		return m_NodeList;
	}

	std::shared_ptr<CNode> C3DObject::FindNodeByName(const std::string& Name)
	{
		std::shared_ptr<CNode> DstNode = nullptr;

		for (const auto& Node : m_NodeList)
		{
			if (Node->GetName() == Name)
			{
				DstNode = Node;
				break;
			}
		}

		return DstNode;
	}

	std::shared_ptr<CNode> C3DObject::FindNodeByIndex(int Index)
	{
		std::shared_ptr<CNode> DstNode = nullptr;

		if (Index >= 0 && Index < static_cast<int>(m_NodeList.size()))
		{
			DstNode = m_NodeList[Index];
		}

		return DstNode;
	}

	void C3DObject::AddMesh(const std::shared_ptr<graphics::CMesh>& Mesh)
	{
		m_MeshList.push_back(Mesh);
	}

	const std::vector<std::shared_ptr<graphics::CMesh>>& C3DObject::GetMeshList() const
	{
		return m_MeshList;
	}

	void C3DObject::AddMorphNode(const std::shared_ptr<CNode>& Node)
	{
		m_MorphController->AddMorphNode(Node);
	}

#ifdef USE_ANIMATION
	const std::shared_ptr<animation::CAnimationController>& C3DObject::GetAnimationController() const
	{
		return m_AnimationController;
	}

	const std::shared_ptr<animation::CBlendShapeController>& C3DObject::GetBlendShapeController() const
	{
		return m_BlendShapeController;
	}

	void C3DObject::AddBlendShapeNode(const std::shared_ptr<CNode>& Node)
	{
		AddMorphNode(Node);

		m_BlendShapeController->AddBlendShapeNode(Node);
	}

	void C3DObject::SetAnimationSkeleton(const std::shared_ptr<animation::CSkeleton >& Skeleton)
	{
		m_AnimationController->SetAnimationSkeleton(Skeleton);
	}

	void C3DObject::AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& Clip, const std::string& MotionName, animation::SAnimationLayout Layout, bool IsLoop)
	{
		m_AnimationController->AddAnimationClip(Clip, MotionName, Layout, IsLoop);
	}

	void C3DObject::AddHumanoidAnimationClip(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::string& MotionName, 
		animation::SAnimationLayout Layout, bool IsLoop, bool UseIK)
	{
		m_AnimationController->AddHumanoidAnimationClip(SourceClip, MotionName, Layout, IsLoop, UseIK);
	}

	void C3DObject::AddBlendShapeClip(const std::shared_ptr<animation::CBlendShapeClip>& Clip, const std::string& MotionName, bool IsLoop)
	{
		m_BlendShapeController->AddBlendShapeClip(Clip, MotionName, IsLoop);
	}

	const std::vector<std::shared_ptr<animation::CAnimationClip>>& C3DObject::GetAnimationClipList() const
	{
		return m_AnimationController->GetAnimationClipList();
	}

	bool C3DObject::IsPlayingAnimation() const
	{
		return m_AnimationController->IsPlayingAnimation();
	}
#endif

	void C3DObject::SetRootNodeIndexList(const std::vector<int>& RootNodeIndexList)
	{
		m_RootNodeIndexList = RootNodeIndexList;
	}

	void C3DObject::AddRootNodeIndex(int Index)
	{
		m_RootNodeIndexList.push_back(Index);
	}

	const std::vector<int>& C3DObject::GetRootNodeIndexList() const
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

#ifdef USE_ANIMATION
	void C3DObject::ChangeMotion(int Index)
	{
		m_AnimationController->ChangeMotion(Index);
	}

	void C3DObject::ChangeMotion(const std::string& MotionName)
	{
		m_AnimationController->ChangeMotion(MotionName);
	}

	void C3DObject::PlayBlendShape(const std::string& MotionName)
	{
		m_BlendShapeController->PlayBlendShape(MotionName);
	}

	void C3DObject::StopBlendShape(const std::string& MotionName)
	{
		m_BlendShapeController->StopBlendShape(MotionName);
	}
#endif // USE_ANIMATION

	const std::shared_ptr<graphics::CTextureSet>& C3DObject::GetTextureSet() const
	{
		return m_TextureSet;
	}
}