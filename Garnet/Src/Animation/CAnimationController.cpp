#ifdef USE_ANIMATION

#include "CAnimationController.h"
#include "../../Message/Console.h"

namespace animation
{
	CAnimationController::CAnimationController():
		m_MaxBlendingTime(0.5f),
		m_CurrBlendingTime(0.0f),
		m_SavedPrevTrs(false),
		m_Skeleton(nullptr),
		m_CurrentLayout(SAnimationLayout()),
		m_CurrentMotionIndex(-1),
		m_CurrentMotionName(std::string())
	{
	}

	CAnimationController::~CAnimationController()
	{
		Reset();
	}

	void CAnimationController::Reset()
	{
		m_CurrBlendingTime = 0.0f;
		m_SavedPrevTrs = false;
		m_CurrentLayout = {};
		m_CurrentMotionIndex = -1;
		m_CurrentMotionName = std::string();
	}

	bool CAnimationController::Update(float DeltaSecondsTime)
	{
		// アニメーションの計算
		const auto& Clip = m_CurrentLayout.Clip;
		if (!Clip) return true;

		if (Clip->IsEnd() && !Clip->IsLoop())
		{
			// アニメーションが終了しているので次のアニメーションに遷移する
			const std::string& NextClipName = m_CurrentLayout.NextClipName;

			if (!NextClipName.empty())
			{
				ChangeMotion(NextClipName);

				return true;
			}
		}
		else
		{
			if (!Clip->Update(DeltaSecondsTime)) return false;

			// モーションブレンド
			if (!BlendMotion(DeltaSecondsTime)) return false;
		}

		return true;
	}

	// IKの計算
	bool CAnimationController::CalculateIK(const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		const auto& CurrentClip = m_CurrentLayout.Clip;

		if (!CurrentClip || !m_Skeleton) return true;

		if (CurrentClip->IsUseIK())
		{
			if (!m_Skeleton->SolveIK()) return false;
		}

		return true;
	}

	// 付与ボーンの計算
	bool CAnimationController::CalculateGrantBone(const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		// 付与はローカルトランスフォームに対して実行する
		if(m_Skeleton)
		{
			const auto& BoneList = m_Skeleton->GetBoneList();

			for (const auto& GrantBone : m_Skeleton->GetGrantBoneList())
			{
				// ParentGrantBoneを取得
				int GrantParentBoneIndex = GrantBone->GetGrantParentBoneIndex();
				if (GrantParentBoneIndex < 0 || GrantParentBoneIndex >= BoneList.size()) continue;

				const auto& ParentGrantBone = BoneList[GrantParentBoneIndex];
				if (!ParentGrantBone) continue;

				// 付与率
				const float GrantRate = GrantBone->GetGrantRate();

				// ひとまず負の時はスキップする
				if (GrantRate < 0.0f) continue;

				if (GrantBone->IsRotateGrant())
				{
					// 回転付与

					glm::quat LocalParentRot = ParentGrantBone->GetBoneNode()->GetRot();

					if (GrantRate >= 0.0f)
					{
						//glm::quat GrantRot = (GrantRate * LocalParentRot) * GrantBone->GetBoneNode()->GetRot();
						glm::quat GrantRot = glm::slerp(GrantBone->GetBoneNode()->GetRot(), LocalParentRot, GrantRate);

						GrantBone->GetBoneNode()->SetRot(GrantRot);
					}
					else
					{
						// 付与率が負の時は逆行列をかける
						glm::quat GrantRot = (fabsf(GrantRate) * glm::inverse(LocalParentRot)) * GrantBone->GetBoneNode()->GetRot();

						GrantBone->GetBoneNode()->SetRot(GrantRot);
					}
				}
				else if (GrantBone->IsMoveGrant())
				{
					glm::vec3 LocalParentPos = ParentGrantBone->GetBoneNode()->GetPos();

					// 移動付与
					if (GrantRate >= 0.0f)
					{
						//glm::vec3 GrantPos = GrantRate * LocalParentPos + GrantBone->GetBoneNode()->GetPos();
						glm::vec3 GrantPos = (1.0f - GrantRate) * GrantBone->GetBoneNode()->GetPos() + GrantRate * LocalParentPos;

						GrantBone->GetBoneNode()->SetPos(GrantPos);
					}
					else
					{
						// 付与率が負の時は逆行列をかける
						glm::vec3 GrantPos = (-1.0f) * GrantRate * LocalParentPos + GrantBone->GetBoneNode()->GetPos();

						GrantBone->GetBoneNode()->SetPos(GrantPos);
					}
				}

				// 付与ボーンのワールド行列を再計算
				glm::mat4 WorldMatrix = glm::mat4();
				const auto& ParentNode = GrantBone->GetBoneNode()->GetParentNode();
				if (ParentNode)
				{
					WorldMatrix = ParentNode->GetWorldMatrix() * GrantBone->GetBoneNode()->GetLocalMatrix();
				}
				else
				{
					WorldMatrix = GrantBone->GetBoneNode()->GetLocalMatrix();
				}

				GrantBone->GetBoneNode()->SetWorldMatrix(WorldMatrix);
			}
		}

		return true;
	}

	void CAnimationController::CalcWorldMatrix(const glm::mat4& ParentWorldMatrix, const std::shared_ptr<object::CNode>& Node, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		glm::mat4 WorldMatrix = ParentWorldMatrix * Node->GetLocalMatrix();
		Node->SetWorldMatrix(WorldMatrix);

		for (int ChildIndex : Node->GetChildrenNodeIndexList())
		{
			if (ChildIndex < 0 || ChildIndex >= NodeList.size()) continue;

			const auto& ChildNode = NodeList[ChildIndex];

			CalcWorldMatrix(WorldMatrix, ChildNode, NodeList);
		}
	}

	// インデックス指定でモーションを変更
	void CAnimationController::ChangeMotion(int Index)
	{
		Reset();

		// 初期化
		if (Index >= 0 && Index < m_ClipList.size())
		{
			const auto& Clip = m_ClipList[Index];
			Clip->Initialize();

			m_CurrentMotionIndex = Index;
			m_CurrentLayout.Clip = Clip;
		}
	}

	// 名前指定でモーションを変更
	void CAnimationController::ChangeMotion(const std::string& MotionName)
	{
		Reset();

		// 初期化
		const auto& Layout = m_ClipMap.find(MotionName);
		if (Layout != m_ClipMap.end())
		{
			m_CurrentMotionName = MotionName;
			m_CurrentLayout = Layout->second;

			const auto& Clip = m_CurrentLayout.Clip;
			if(Clip) Clip->Initialize();
		}

		// 現在の姿勢を保存する
		if (!m_SavedPrevTrs)
		{
			if(m_Skeleton)
			{
				for (const auto& Bone : m_Skeleton->GetBoneList())
				{
					Bone->GetBoneNode()->SavePrevLocalTransform();
				}
			}

			m_SavedPrevTrs = true;
		}
	}

	bool CAnimationController::CalCSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix)
	{
		//if (IsPlayingAnimation())
		if (IsEnabledSkeleton())
		{
			if(m_Skeleton)
			{
				if (!m_Skeleton->CalCSkinMatrixList(MatrixList, ObjectModelMatrix)) return false;
			}
		}

		return true;
	}

	bool CAnimationController::IsEnabledSkeleton()
	{
		return (m_Skeleton != nullptr && m_Skeleton->GetBoneList().size() > 0);
	}

	bool CAnimationController::IsPlayingAnimation()
	{
		return (m_CurrentLayout.Clip != nullptr && !m_CurrentLayout.Clip->IsEnd());
	}

	int CAnimationController::GetCurrentMotionIndex() const
	{
		return m_CurrentMotionIndex;
	}

	const std::string& CAnimationController::GetCurrentMotionName() const
	{
		return m_CurrentMotionName;
	}

	void CAnimationController::AddMotion(const std::string& MotionName, animation::SAnimationLayout Layout)
	{
		m_ClipMap.emplace(MotionName, Layout);
		m_ClipList.push_back(Layout.Clip);
	}

	void CAnimationController::SetAnimationSkeleton(const std::shared_ptr<animation::CSkeleton>& Skeleton)
	{
		m_Skeleton = Skeleton;
	}

	const std::shared_ptr<animation::CSkeleton>& CAnimationController::GetSkeleton() const
	{
		return m_Skeleton;
	}

	void CAnimationController::AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& Clip)
	{
		m_ClipList.push_back(Clip);
	}

	void CAnimationController::AddHumanoidAnimationClip(const std::shared_ptr<animation::CAnimationClip>& SourceClip, 
		const std::string& MotionName, animation::SAnimationLayout Layout, bool IsLoop, bool UseIK)
	{
		if (!m_Skeleton) return;

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

				TargetKeyFrame->SetXPointList(SourceKeyFrame->GetXPointList());
				TargetKeyFrame->SetYPointList(SourceKeyFrame->GetYPointList());
				TargetKeyFrame->SetZPointList(SourceKeyFrame->GetZPointList());
				TargetKeyFrame->SetRPointList(SourceKeyFrame->GetRPointList());

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

			for (const auto& Bone : m_Skeleton->GetBoneList())
			{
				if (Bone->GetBoneName() == animation::EHumanoidBones::None) continue;

				if (Bone->GetBoneName() == SourceChannel->GetBoneName())
				{
					TargetNode = Bone->GetBoneNode();

					break;
				}
			}

			std::shared_ptr<animation::CAnimationChannel> TargetChannel = std::make_shared<animation::CAnimationChannel>(SourceChannel->IsUseAnimLocalAxis(), SourceChannel->IsTransOffset(), SourceChannel->GetSamplerIndex(), SourceChannel->GetAnimationTarget(), TargetNode, SourceChannel->GetBoneName());

			TargetClip->AddAnimationChannel(TargetChannel);
		}

		// DefaultSkeletonを持っている時のみリターゲットを行う
		// リターゲットは平行移動成分(Pos)に対して行うものなので、回転だけのアニメーションには必要ない
		if (SourceClip->GetDefaultSkeleton())
		{
			// RigのReTargetingを行う
			// リターゲティングとはリグの形が異なるアニメーションを自身のアニメーションに合うように調整すること
			// 例えば身長が違うとアバターが伸びてしまう
			if (!ReTargetRig(SourceClip, TargetClip)) return;
		}

		TargetClip->SetIsLoop(IsLoop);
		TargetClip->SetUseIK(UseIK);

		Layout.Clip = TargetClip;
		AddMotion(MotionName, Layout);
	}

	const std::vector<std::shared_ptr<animation::CAnimationClip>>& CAnimationController::GetAnimationClipList() const
	{
		return m_ClipList;
	}

	const std::unordered_map<std::string, SAnimationLayout>& CAnimationController::GetAnimationClipMap() const
	{
		return m_ClipMap;
	}

	bool CAnimationController::BlendMotion(float DeltaSecondsTime)
	{
		if (m_CurrBlendingTime < m_MaxBlendingTime)
		{
			// 時間更新
			m_CurrBlendingTime += DeltaSecondsTime;

			m_CurrBlendingTime = fmaxf(m_CurrBlendingTime, 0.0f);
			m_CurrBlendingTime = fminf(m_CurrBlendingTime, m_MaxBlendingTime);

			// 現在の姿勢と遷移前の姿勢を補完する
			std::vector<std::shared_ptr<object::CNode>> ComputedNodeList;
			float L = 1.0f - (m_MaxBlendingTime - m_CurrBlendingTime) / m_MaxBlendingTime;

			if(m_Skeleton)
			{
				for (const auto& Bone : m_Skeleton->GetBoneList())
				{
					const auto& Node = Bone->GetBoneNode();

					BlendTranslation(Node, L);
					BlendRotation(Node, L);
				}
			}
		}

		return true;
	}

	void CAnimationController::BlendTranslation(const std::shared_ptr<object::CNode>& Node, float L)
	{
		const glm::vec3& PrevPos = Node->GetPrevLocalTransform()->GetPos();
		const glm::vec3& NextPos = Node->GetLocalTransform()->GetPos();
		
		glm::vec3 CurrPos = glm::vec3(0.0f);

		CurrPos.x = (1.0f - L) * PrevPos.x + L * NextPos.x;
		CurrPos.y = (1.0f - L) * PrevPos.y + L * NextPos.y;
		CurrPos.z = (1.0f - L) * PrevPos.z + L * NextPos.z;

		Node->GetLocalTransform()->SetPos(CurrPos);
	}

	void CAnimationController::BlendRotation(const std::shared_ptr<object::CNode>& Node, float L)
	{
		const glm::quat& PrevRot = Node->GetPrevLocalTransform()->GetRot();
		const glm::quat& NextRot = Node->GetLocalTransform()->GetRot();

		glm::quat CurrRot = glm::slerp(PrevRot, NextRot, L);

		Node->GetLocalTransform()->SetRot(CurrRot);
	}

	bool CAnimationController::ReTargetRig(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::shared_ptr<animation::CAnimationClip>& TargetClip)
	{
		const auto& SourceSkeleton = SourceClip->GetDefaultSkeleton();
		if (!SourceSkeleton) return false;

		// Rigのリターゲティングを実行する
		for (const auto& TargetChannel : TargetClip->GetChannelList())
		{
			int TargetSamplerIndex = TargetChannel->GetSamplerIndex();
			if (TargetSamplerIndex < 0 || TargetSamplerIndex >= TargetClip->GetSamplerList().size()) continue;

			const auto& TargetSampler = TargetClip->GetSamplerList()[TargetSamplerIndex];

			animation::EHumanoidBones BoneName = TargetChannel->GetBoneName();

			animation::EAnimationTarget AnimationTarget = TargetChannel->GetAnimationTarget();

			// BoneTableに登録されていないものについては処理の対象外とする
			if (BoneName == animation::EHumanoidBones::None) continue;

			const auto& SourceBone = SourceSkeleton->GetBone(BoneName);
			if (!SourceBone) continue;

			const glm::mat4 SourceRestMove = SourceBone->GetBoneNode()->GetDefaultLocalMoveMatrix();
			const glm::mat4 InverseSourceRestMove = glm::inverse(SourceRestMove);

			const auto& TargetBone = m_Skeleton->GetBone(BoneName);
			if (!TargetBone) continue;

			const glm::mat4 TargetRestMove = TargetBone->GetBoneNode()->GetDefaultLocalMoveMatrix();

			// SourceとTargetのバインドマトリックスのTranslationの差分を示す行列
			const glm::mat4 ReTargetTranslationMatrix = TargetRestMove * InverseSourceRestMove;

			for (const auto& TargetKeyFrame : TargetSampler->GetKeyFrameList())
			{
				const float CurrentTime = TargetKeyFrame->GetInput();

				if (AnimationTarget == animation::EAnimationTarget::MODELMATRIX)
				{
					glm::mat4 SourcePose = glm::mat4(1.0f);
					TargetKeyFrame->GetOutput(&SourcePose[0][0]);

					glm::mat4 TargetPose = ReTargetTranslationMatrix * SourcePose;

					TargetKeyFrame->SetOutput(&TargetPose[0][0], sizeof(glm::mat4));
				}
				else if (AnimationTarget == animation::EAnimationTarget::TRANSLATION)
				{
					glm::vec3 SourceTranslation = glm::vec3(1.0f);
					TargetKeyFrame->GetOutput(&SourceTranslation[0]);

					glm::mat4 SourcePose = glm::translate(glm::mat4(1.0f), SourceTranslation);

					glm::mat4 TargetPose = ReTargetTranslationMatrix * SourcePose;

					glm::vec3 TargetTranslation = glm::vec3(1.0f);
					math::CTransform::CastModelMatrixToTranslation(TargetPose, TargetTranslation);

					TargetKeyFrame->SetOutput(&TargetTranslation[0], sizeof(glm::vec3));
				}
				else
				{
					// リターゲットはリグの長さの違いを補正するためのものなのでMODELMATRIXとTRANSLATIONに対してのみ行う
					continue;
				}
			}
		}

		return true;
	}
}

#endif