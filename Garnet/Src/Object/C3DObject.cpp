#include "C3DObject.h"

namespace object
{
	C3DObject::C3DObject(const std::string& PassName, const std::string& DepthPassName):
		m_IsCreated(false),
		m_FileName(std::string()),
		m_DefaultMaterialFrame(std::string()),
		m_PassName(PassName),
		m_DepthPassName(DepthPassName),
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
		m_MaterialList.clear();
	}

	void C3DObject::SetFileName(const std::string& Name)
	{
		m_FileName = Name;
	}

	const std::string& C3DObject::GetFileName() const
	{
		return m_FileName;
	}

	void C3DObject::SetDefaultMaterialFrame(const std::string& Name)
	{
		m_DefaultMaterialFrame = Name;
	}

	const std::string& C3DObject::GetDefaultMaterialFrame() const
	{
		return m_DefaultMaterialFrame;
	}

	void C3DObject::SetObjectName(const std::string& Name)
	{
		m_ObjectName = Name;
	}
	const std::string& C3DObject::GetObjectName() const
	{
		return m_ObjectName;
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
	
	bool C3DObject::CreatePresetSimply(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine,
		const std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>>& createInfo,
		graphics::EPresetPrimitiveType PresetType,
		const std::shared_ptr<graphics::CMaterial>& Material, const std::shared_ptr<graphics::CMaterialFrame>& DepthMF, 
		const std::shared_ptr<math::CTransform> NodeTransform, const std::shared_ptr<physics::IPhysicsObject>& PhysicsObject)
	{
		// Material
		AddMaterial(Material);

		// Mesh
		std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
		Mesh->CreatePresetSimpleMesh(createInfo.first, createInfo.second, 0, PresetType);

		AddMesh(Mesh);

		// Node
		std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, 0);
		Node->SetLocalTransform(NodeTransform);
		Node->AddPhysicsObject(PhysicsObject);
		AddNode(Node);

		// Create
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
		bool ExistMorph = false;

		for (const auto& Mesh : m_MeshList)
		{
			if (!Mesh->CreateBuffer()) return false;

			// モーフ処理が必要かどうか
			if (Mesh->GetMorphDataList().size() > 0)
			{
				ExistMorph = true;
			}

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

		// モーフ
		if (ExistMorph)
		{
			if (!m_MorphController->Create(m_MeshList)) return false;
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
		if (!m_BlendShapeController->Update(DeltaSecondsTime)) return false;

		// IKの計算を行う
		if (!m_AnimationController->CalculateIK(m_NodeList)) return false;

		// 付与ボーンの位置を計算
		if (!m_AnimationController->CalculateGrantBone(m_NodeList)) return false;

		// モーフ
		if (!m_MorphController->Update(DeltaSecondsTime, m_MeshList)) return false;
#endif
		// ワールド行列の更新
		// 全ノードマイフレーム更新しているので、そのうちキャッシュを入れて更新は必要なものだけにする
		CalcWorldMatrix();

		// 物理ジョイントの位置をボーン位置に合わせる
		//AlignPhysicsJoint();

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

	bool C3DObject::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, bool DrawOutline, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo,
		const std::shared_ptr<object::C3DObject>& DebugSphere)
	{
		if (!m_IsCreated) return true;

		if (!m_Enabled) return true;

		// 描画パスが違うなら描画しない
		// ToDo: PassNameは配列にしてもいいかもしれない
		if (IsDepthPass)
		{
			if (m_DepthPassName != pGraphicsAPI->GetCurrentRenderPassName()) return true;
		}
		else
		{
			if (m_PassName != pGraphicsAPI->GetCurrentRenderPassName()) return true;
		}

		// 描画
		for (const auto& Node : m_NodeList)
		{
			if (!Node->IsEnabled() || !Node->IsDrawable()) continue;

			int MeshIndex = Node->GetMeshIndex();
			if (MeshIndex < 0 || MeshIndex >= m_MeshList.size()) continue;

			const auto& WorldMatrix = m_ObjectTransform->GetModelMatrix() * Node->GetWorldMatrix();
			const auto& InvWorldMatrix = glm::inverse(WorldMatrix);
			const auto& Mesh = m_MeshList[MeshIndex];

			int SkeletonIndex = Node->GetSkeletonIndex();

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
				Material->SetUniformValue("invModel", &InvWorldMatrix[0][0], sizeof(glm::mat4), DynamicOffsetNum);
				Material->SetUniformValue("view", &Camera->GetViewMatrix()[0][0], sizeof(glm::mat4), DynamicOffsetNum);
				Material->SetUniformValue("proj", &Projection->GetPrejectionMatrix()[0][0], sizeof(glm::mat4), DynamicOffsetNum);
				Material->SetUniformValue("lightVPMat", &lightVPMat[0][0], sizeof(glm::mat4), DynamicOffsetNum);
				glm::vec3 lightDir = DrawInfo->GetLightCamera()->GetViewDir();
				Material->SetUniformValue("lightDir", &glm::vec4(lightDir.x, lightDir.y, lightDir.z, 0.0f)[0], sizeof(glm::vec4), DynamicOffsetNum);
				Material->SetUniformValue("lightColor", &DrawInfo->GetLightColor()[0], sizeof(glm::vec4), DynamicOffsetNum);
				glm::vec3 CameraPos = Camera->GetPos();
				Material->SetUniformValue("cameraPos", &glm::vec4(CameraPos.x, CameraPos.y, CameraPos.z, 1.0f)[0], sizeof(glm::vec4), DynamicOffsetNum);
				Material->SetUniformValue("time", &glm::vec1(DrawInfo->GetSecondsTime())[0], sizeof(float), DynamicOffsetNum);
				Material->SetUniformValue("deltaTime", &glm::vec1(DrawInfo->GetDeltaSecondsTime())[0], sizeof(float), DynamicOffsetNum);
				Material->SetUniformValue("resolution", &Projection->GetScreenResolution()[0], sizeof(glm::vec2), DynamicOffsetNum);
#ifdef USE_ANIMATION
				Material->SetUniformValue("useSkinMeshAnimation", &glm::ivec1((m_AnimationController->IsEnabledSkeleton() ? 1 : 0))[0], sizeof(glm::ivec1), DynamicOffsetNum);

				// SkinMatrixをShaderに渡す
				if (m_CurrentSkinMatrixList.size() > 0)
				{
					Material->SetUniformValue("r_SkinMatrixBuffer", &m_CurrentSkinMatrixList[0], sizeof(glm::mat4) * static_cast<int>(m_CurrentSkinMatrixList.size()), DynamicOffsetNum);
				}
#endif
				// 描画実行
				if (!Primitive->Draw(Material, DynamicOffsetNum, IsDepthPass)) return false;

				// マテリアルの参照カウントをインクリメントする
				Material->IncreaseDynamicOffset();

				// 描画準備のために変更した設定を元に戻す
				Material->ResetToDefaultCullMode();
			}
		}

		//if (!DrawDebugBone(IsDepthPass, DrawOutline, Camera, Projection, DrawInfo, DebugSphere)) return false;
		//if (!DrawDebugPhysics(IsDepthPass, DrawOutline, Camera, Projection, DrawInfo, DebugSphere)) return false;

		return true;
	}

	bool C3DObject::DrawDebugBone(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, bool DrawOutline, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
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

					const auto& BoneNode = Bone->GetBoneNode();

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

					DebugSphere->GetMaterialList()[0]->SetUniformValue("useColor", &glm::ivec1(1)[0], sizeof(glm::ivec1));
					DebugSphere->GetMaterialList()[0]->SetUniformValue("baseColor", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0], sizeof(glm::vec4));
					DebugSphere->GetMaterialList()[0]->SetUniformValue("baseColorFactor", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0], sizeof(glm::vec4));

					if (!DebugSphere->Draw(pGraphicsAPI, IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
				}
			}
		}
#endif
		return true;
	}

	bool C3DObject::DrawDebugPhysics(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, bool DrawOutline, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
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
					if (Bone->GetBoneName() == animation::EHumanoidBones::Center) continue;

					const auto& BoneNode = Bone->GetBoneNode();

					DebugSphere->GetMaterialList()[0]->SetUniformValue("useColor", &glm::ivec1(1)[0], sizeof(glm::ivec1));

					for (const auto& PhysicsObject : BoneNode->GetPhysicsObjectList())
					{
						if (PhysicsObject->IsStatic())
						{
							DebugSphere->GetMaterialList()[0]->SetUniformValue("baseColor", &glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)[0], sizeof(glm::vec4));
						}
						else
						{
							DebugSphere->GetMaterialList()[0]->SetUniformValue("baseColor", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0], sizeof(glm::vec4));
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

						if (!DebugSphere->Draw(pGraphicsAPI, IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
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

	void C3DObject::AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& Clip)
	{
		m_AnimationController->AddAnimationClip(Clip);
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

	const std::vector<std::shared_ptr<graphics::CMaterial>>& C3DObject::GetMaterialList() const
	{
		return m_MaterialList;
	}

	bool C3DObject::ReplaceMaterial(const std::shared_ptr<graphics::CMaterial>& OldMaterial, const std::shared_ptr<graphics::CMaterial>& NewMaterial)
	{
		auto it = std::find(m_MaterialList.begin(), m_MaterialList.end(), OldMaterial);
		if (it == m_MaterialList.end()) return false;

		OldMaterial->DeleteMaterialFrameReference();

		int MaterialIndex = static_cast<int>(it - m_MaterialList.begin());

		if (!NewMaterial->Create(m_TextureSet)) return false;

		m_MaterialList[MaterialIndex] = NewMaterial;

		return true;
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

	void C3DObject::PlayBlendShape(const std::string& MotionName)
	{
		m_BlendShapeController->PlayBlendShape(MotionName);
	}

	void C3DObject::StopBlendShape(const std::string& MotionName)
	{
		m_BlendShapeController->StopBlendShape(MotionName);
	}

	const std::shared_ptr<graphics::CTextureSet>& C3DObject::GetTextureSet() const
	{
		return m_TextureSet;
	}
}