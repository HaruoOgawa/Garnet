#include "C3DObject.h"

namespace object
{
	C3DObject::C3DObject(const std::string& PassName, const std::string& DepthPassName):
		m_PassName(PassName),
		m_DepthPassName(DepthPassName),
		m_ObjectTransform(std::make_shared<math::CTransform>()),
		m_CurrentClipIndex(-1),
		m_TotalJointIndexOffset(0)
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

		// DefaultLocalTransformを保存する
		ApplyDefaultLocalTransform();

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

		// SSBOのサイズをDynamicOffset毎に変更できるかわからないのでひとまず全部まとめて渡す
		std::vector<glm::mat4> SkinMatrixList;
		if (IsPlayingAnimation())
		{
			for (const auto& Skin : m_AnimationSkinList)
			{
				if (!Skin->CalcSkinMatrixList(SkinMatrixList, m_ObjectTransform->GetModelMatrix())) return false;
			}
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
			int SkinIndex = Node->GetSkinIndex();

			/*std::vector<glm::mat4> SkinMatrixList;
			if (SkinIndex >= 0 && SkinIndex < m_AnimationSkinList.size() && IsPlayingAnimation())
			{
				const auto& Skin = m_AnimationSkinList[SkinIndex];
				
				if (!Skin->CalcSkinMatrixList(SkinMatrixList, m_ObjectTransform->GetModelMatrix())) return false;
			}*/

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

					int JointIndexOffset = m_AnimationSkinList[SkinIndex]->GetJointIndexOffset();
					Material->SetUniformValue("JointIndexOffset", &glm::ivec1(JointIndexOffset)[0], DynamicOffsetNum);
				}

				if (!Primitive->Draw(Material, DynamicOffsetNum, IsDepthPass)) return false;
			}
		}
		
		for (const auto& Skin : m_AnimationSkinList)
		{
			
			for (const auto& Joint : Skin->GetJointList())
			{
				// Debug用: Jointの描画
				const auto& JointNode = Joint->GetJointNode();
				DebugSphere->SetPos(m_ObjectTransform->GetModelMatrix() * JointNode->GetWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
				DebugSphere->SetScale(glm::vec3(0.25f));

				if (JointNode->GetName() == "mixamorig:Hips")
				{
					DebugSphere->GetMaterialList()[0]->SetUniformValue("baseColor", &glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)[0]);
				}
				else if (JointNode->GetName() == "mixamorig:RightUpLeg" || JointNode->GetName() == "mixamorig:LeftUpLeg" || JointNode->GetName() == "mixamorig:Spine")
				{
					DebugSphere->GetMaterialList()[0]->SetUniformValue("baseColor", &glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)[0]);
				}
				else if (JointNode->GetName() == "mixamorig:LeftLeg" || JointNode->GetName() == "mixamorig:RightLeg" || JointNode->GetName() == "mixamorig:Spine1")
				{
					DebugSphere->GetMaterialList()[0]->SetUniformValue("baseColor", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0]);
				}
				else
				{
					DebugSphere->GetMaterialList()[0]->SetUniformValue("baseColor", &glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)[0]);
				}

				if (!DebugSphere->Draw(IsDepthPass, Camera, Projection, DrawInfo)) return false;

				// Debug用: Boneの描画
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
		int JointIndexOffset = m_TotalJointIndexOffset;

		Skin->SetJointIndexOffset(JointIndexOffset);

		m_AnimationSkinList.push_back(Skin);

		m_TotalJointIndexOffset += static_cast<int>(Skin->GetJointList().size());
	}

	void C3DObject::AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& Clip)
	{
		m_AnimationClipList.push_back(Clip);
	}

	void C3DObject::AddHumanoidAnimationClip(const std::shared_ptr<animation::CAnimationClip>& SourceClip)
	{
		// Clipの値をコピーする
		std::shared_ptr<animation::CAnimationClip> TargetClip = std::make_shared<animation::CAnimationClip>();

		// samplers
		for (const auto& SourceSampler : SourceClip->GetSamplerList())
		{
			std::shared_ptr<animation::CAnimationSampler> TargetSampler = std::make_shared<animation::CAnimationSampler>(SourceSampler->GetInterpolationType());

			for (const auto& SourceKeyFrame : SourceSampler->GetKeyFrameList())
			{
				std::shared_ptr<animation::CKeyFrame> TargetKeyFrame = std::make_shared<animation::CKeyFrame>(SourceKeyFrame->GetType());

				TargetKeyFrame->SetInput(SourceKeyFrame->GetInput());

				std::vector<float> TargetOutput = SourceKeyFrame->GetOutput();
				TargetKeyFrame->SetOutput(TargetOutput);

				TargetSampler->AddKeyFrame(TargetKeyFrame);
			}

			TargetSampler->SetStartTime(SourceSampler->GetStartTime());
			TargetSampler->SetEndTime(SourceSampler->GetEndTime());

			TargetClip->AddAnimationSampler(TargetSampler);
		}
		
		// channels
		// 同じ名前のノードは一つしかない前提でchannelを作成する
		for (const auto& SourceChannel : SourceClip->GetChannelList())
		{
			std::shared_ptr<object::CNode> TargetNode = nullptr;

			for (const auto& Skin : m_AnimationSkinList)
			{
				for (const auto& Joint : Skin->GetJointList())
				{
					if (Joint->GetBoneName() == animation::EHumanoidBones::None) continue;

					if (Joint->GetBoneName() == SourceChannel->GetBoneName())
					{
						TargetNode = Joint->GetJointNode();

						break;
					}
				}

				if (TargetNode)
				{
					break;
				}
			}

			std::shared_ptr<animation::CAnimationChannel> TargetChannel = std::make_shared<animation::CAnimationChannel>(SourceChannel->GetSamplerIndex(), SourceChannel->GetAnimationTarget(), TargetNode, SourceChannel->GetBoneName());

			TargetClip->AddAnimationChannel(TargetChannel);
		}
		
		// RigのReTargetingを行う
		// リターゲティングとはリグの形が異なるアニメーションを自身のアニメーションに合うように調整すること
		// 例えば身長が違うとアバターが伸びてしまうしリグが反対だとねじれてしまう
		if (!ReTargetingRig(SourceClip, TargetClip)) return;

		m_AnimationClipList.push_back(TargetClip);
	}

	bool C3DObject::ReTargetingRig(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::shared_ptr<animation::CAnimationClip>& TargetClip)
	{
		const auto& SourceSkin = SourceClip->GetDefaultSkin();
		if (!SourceSkin) return false;

		// Frameのワールドマトリックスリストを作成
		std::unordered_map<animation::EHumanoidBones, std::vector<std::shared_ptr<animation::CKeyFrame>>> SourceWorldFrameMatrixMap;
		{
			// 一番フレーム数が多いサンプラーをペースメーカーとする
			auto PaceMaker = SourceClip->GetSamplerWithMostFrames();
			if (!PaceMaker) return false;

			for (const auto& KeyFrame : PaceMaker->GetKeyFrameList())
			{
				const float CurrentTime = KeyFrame->GetInput();

				if (!SourceClip->UpdateFrame(CurrentTime)) return false;
				
				SourceSkin->CalcSkinWorldMatrix();

				for (const auto& SourceChannel : SourceClip->GetChannelList())
				{
					int SourceSamplerIndex = SourceChannel->GetSamplerIndex();
					if (SourceSamplerIndex < 0 || SourceSamplerIndex >= SourceClip->GetSamplerList().size()) continue;

					const auto& SourceSampler = SourceClip->GetSamplerList()[SourceSamplerIndex];

					animation::EHumanoidBones BoneName = SourceChannel->GetBoneName();

					// BoneTableに登録されていないものについては処理の対象外とする
					if (BoneName == animation::EHumanoidBones::None) continue;

					{
						const auto& SourceBone = SourceSkin->GetBone(BoneName);
						if (!SourceBone) continue;

						// フレームマトリックスを計算する
						glm::mat4 FrameMatrix = SourceBone->GetJointNode()->GetWorldMatrix();

						// Mapに登録
						auto it = SourceWorldFrameMatrixMap.find(BoneName);
						if (it == SourceWorldFrameMatrixMap.end())
						{
							SourceWorldFrameMatrixMap.emplace(BoneName, std::vector<std::shared_ptr<animation::CKeyFrame>>());
						}

						std::shared_ptr<animation::CKeyFrame> dstKeyFrame = std::make_shared<animation::CKeyFrame>(animation::EKeyFrameType::KEYFRAME_TYPE_MATRIX);
						dstKeyFrame->SetInput(CurrentTime);
						dstKeyFrame->SetOutput(&FrameMatrix[0][0], sizeof(glm::mat4));

						SourceWorldFrameMatrixMap[BoneName].push_back(dstKeyFrame);
					}
				}
			}

			// DefaultTransformに戻す
			SourceSkin->ResetToDefaultSkinLocal();
		}

		// Rigのリターゲティングを実行する
		for (const auto& TargetChannel : TargetClip->GetChannelList())
		{
			int TargetSamplerIndex = TargetChannel->GetSamplerIndex();
			if (TargetSamplerIndex < 0 || TargetSamplerIndex >= TargetClip->GetSamplerList().size()) continue;

			const auto& TargetSampler = TargetClip->GetSamplerList()[TargetSamplerIndex];

			animation::EHumanoidBones BoneName = TargetChannel->GetBoneName();

			// BoneTableに登録されていないものについては処理の対象外とする
			if (BoneName == animation::EHumanoidBones::None) continue;

			const auto& SourceBone = SourceSkin->GetBone(BoneName);
			if (!SourceBone) continue;

			for (const auto& TargetSkin : m_AnimationSkinList)
			{
				const auto& TargetBone = TargetSkin->GetBone(BoneName);
				if (!TargetBone) continue;

				// Targetはアニメーション情報の受け手側(例えばVRMとか). Sourceは送り手側
				// なのでTargetBindMatrixは受け手側のT-Poseのワールドマトリックスを示す
				// そしてここではJointのアニメーション位置を調整する
				const glm::mat4 SourceInverseBindMatrix = glm::inverse(SourceBone->GetJointNode()->CalcDefaultWorldMatrix(SourceBone->GetJointNode()->GetLocalMatrix()));
				const glm::mat4 TargetBindMatrix = TargetBone->GetJointNode()->CalcDefaultWorldMatrix(TargetBone->GetJointNode()->GetLocalMatrix());

				const glm::mat4 TargetInverseParentBindMatrix = glm::inverse(TargetBone->GetJointNode()->CalcDefaultParentWorldMatrix());

				// SourceとTargetのバインドマトリックスの差分を示す行列
				const glm::mat4 ReTargetingMatrix = TargetBindMatrix * SourceInverseBindMatrix;

				for (const auto& TargetKeyFrame : TargetSampler->GetKeyFrameList())
				{
					const float CurrentTime = TargetKeyFrame->GetInput();

					std::vector<float> Value = TargetKeyFrame->GetOutput();

					switch (TargetChannel->GetAnimationTarget())
					{
						// ひとまずMODELMATRIXだけ対応する
					case animation::EAnimationTarget::MODELMATRIX:
					{
						glm::mat4 TargetLocalFrameMatrix = glm::mat4(1.0f);
						std::memcpy(&TargetLocalFrameMatrix[0][0], &Value[0], sizeof(float) * Value.size());

						//glm::mat4 SourceWorldFrameMatrix = SourceBone->GetJointNode()->CalcDefaultWorldMatrix(TargetLocalFrameMatrix);
						//SourceWorldFrameMatrix = SourceBone->GetJointNode()->CalcDefaultWorldMatrix(SourceBone->GetJointNode()->GetDefaultLocalMatrix());
						
						glm::mat4 SourceWorldFrameMatrix = glm::mat4(1.0f);
						{
							auto FrameBonePair = SourceWorldFrameMatrixMap.find(BoneName);
							if (FrameBonePair == SourceWorldFrameMatrixMap.end()) continue;

							std::shared_ptr<animation::CKeyFrame> PrevKeyFrame = nullptr;
							std::shared_ptr<animation::CKeyFrame> NextKeyFrame = nullptr;

							// Next
							const auto& val = std::find_if(FrameBonePair->second.begin(), FrameBonePair->second.end(), [&](std::shared_ptr<animation::CKeyFrame>& f) {  bool r = (CurrentTime <= f->GetInput()); if (r) { NextKeyFrame = f; } return r; });
							if (val == FrameBonePair->second.end()) continue;

							// Prev
							size_t NextIndex = std::distance(FrameBonePair->second.begin(), val);

							// CurrentTimeがKeyFrameの最初よりも小さい時はPrevとNextにそれぞれ0と1のKeyFrameを割り当てる
							if (NextIndex <= 0 || NextIndex >= FrameBonePair->second.size())
							{
								NextKeyFrame = FrameBonePair->second[1];
								PrevKeyFrame = FrameBonePair->second[0];
							}
							else
							{
								PrevKeyFrame = FrameBonePair->second[NextIndex - 1];
							}

							if (PrevKeyFrame == nullptr || NextKeyFrame == nullptr) continue;

							float NextOffset = glm::abs(NextKeyFrame->GetInput() - CurrentTime);
							float PrevOffset = glm::abs(PrevKeyFrame->GetInput() - CurrentTime);

							if (PrevOffset < NextOffset)
							{
								PrevKeyFrame->GetOutput(&SourceWorldFrameMatrix[0][0]);
							}
							else
							{
								NextKeyFrame->GetOutput(&SourceWorldFrameMatrix[0][0]);
							}
						}

						// ワールド座標系でのアニメーション位置をReTargetingMatrixの分だけずらすことでTargetのSkinに合わせた後、ローカル座標系でのアニメーションに戻す
						TargetLocalFrameMatrix = TargetInverseParentBindMatrix * ReTargetingMatrix * SourceWorldFrameMatrix;

						std::memcpy(&Value[0], &TargetLocalFrameMatrix[0][0], sizeof(glm::mat4));
					}
						break;
					default:
						break;
					}

					// 値を再セットする
					TargetKeyFrame->SetOutput(Value);
				}

				// 対象のBoneについては一度しか計算しない
				break;
			}
		}
	
		return true;
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