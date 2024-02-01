#ifdef USE_ANIMATION

#include "CAnimationController.h"

namespace animation
{
	CAnimationController::CAnimationController():
		m_MaxBlendingTime(0.5f),
		m_CurrBlendingTime(0.0f),
		m_SavedPrevTrs(false),
		m_CurrentClipIndex(-1),
		m_CurrentClipName(""),
		m_TotalJointIndexOffset(0)
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
		m_CurrentClipIndex = -1;
		m_CurrentClipName = "";
	}

	bool CAnimationController::Update(float DeltaSecondsTime)
	{
		// アニメーションの計算
		if (m_CurrentClipIndex >= 0 && m_CurrentClipIndex < m_ClipList.size())
		{
			const auto& Clip = m_ClipList[m_CurrentClipIndex];
			if (!Clip->Update(DeltaSecondsTime)) return false;

			// モーションブレンド
			if (!BlendMotion(DeltaSecondsTime)) return false;
		}
		else if (m_ClipMap.find(m_CurrentClipName) != m_ClipMap.end())
		{
			const auto& Layout = m_ClipMap.find(m_CurrentClipName);
			if (Layout != m_ClipMap.end())
			{
				const auto& Clip = Layout->second.Clip;

				if (Clip->IsEnd() && !Clip->IsLoop())
				{
					// アニメーションが終了しているので次のアニメーションに遷移する
					const std::string& NextClipName = Layout->second.NextClipName;

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
			}
		}

		return true;
	}

	// インデックス指定でモーションを変更
	void CAnimationController::ChangeMotion(int Index)
	{
		Reset();

		m_CurrentClipIndex = Index;

		// 初期化
		if (m_CurrentClipIndex >= 0 && m_CurrentClipIndex < m_ClipList.size())
		{
			const auto& Clip = m_ClipList[m_CurrentClipIndex];
			Clip->Initialize();
		}
	}

	// 名前指定でモーションを変更
	void CAnimationController::ChangeMotion(const std::string& MotionName)
	{
		Reset();

		m_CurrentClipName = MotionName;

		// 初期化
		if (m_ClipMap.find(m_CurrentClipName) != m_ClipMap.end())
		{
			const auto& Clip = m_ClipMap.find(m_CurrentClipName);
			if (Clip != m_ClipMap.end())
			{
				Clip->second.Clip->Initialize();
			}
		}

		// 現在の姿勢を保存する
		if (!m_SavedPrevTrs)
		{
			for (const auto& Skin : m_SkinList)
			{
				for (const auto& Joint : Skin->GetJointList())
				{
					Joint->GetJointNode()->SavePrevLocalTransform();
				}
			}

			m_SavedPrevTrs = true;
		}
	}

	bool CAnimationController::CalcSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix)
	{
		if (IsPlayingAnimation())
		{
			for (const auto& Skin : m_SkinList)
			{
				if (!Skin->CalcSkinMatrixList(MatrixList, ObjectModelMatrix)) return false;
			}
		}

		return true;
	}

	bool CAnimationController::IsPlayingAnimation()
	{
		bool result = false;

		if (m_CurrentClipIndex >= 0 && m_CurrentClipIndex < m_ClipList.size())
		{
			result = true;
		}
		else if (m_ClipMap.find(m_CurrentClipName) != m_ClipMap.end())
		{
			result = true;
		}

		return result;
	}

	void CAnimationController::AddMotion(const std::string& MotionName, animation::SAnimationLayout Layout)
	{
		m_ClipMap.emplace(MotionName, Layout);
		m_ClipList.push_back(Layout.Clip);
	}

	void CAnimationController::AddAnimationSkin(const std::shared_ptr<animation::CSkin>& Skin)
	{
		int JointIndexOffset = m_TotalJointIndexOffset;

		Skin->SetJointIndexOffset(JointIndexOffset);

		m_SkinList.push_back(Skin);

		m_TotalJointIndexOffset += static_cast<int>(Skin->GetJointList().size());
	}

	const std::vector<std::shared_ptr<animation::CSkin>>& CAnimationController::GetSkinList() const
	{
		return m_SkinList;
	}

	void CAnimationController::AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& Clip)
	{
		m_ClipList.push_back(Clip);
	}

	void CAnimationController::AddHumanoidAnimationClip(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::string& MotionName, animation::SAnimationLayout Layout, bool IsLoop)
	{
		// IsWorldAnim: アニメーションがワールド座標系のデータを示すかどうか

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

			for (const auto& Skin : m_SkinList)
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

			std::shared_ptr<animation::CAnimationChannel> TargetChannel = std::make_shared<animation::CAnimationChannel>(SourceChannel->IsUseAnimLocalAxis(), SourceChannel->IsTransOffset(), SourceChannel->GetSamplerIndex(), SourceChannel->GetAnimationTarget(), TargetNode, SourceChannel->GetBoneName());

			TargetClip->AddAnimationChannel(TargetChannel);
		}

		// DefaultSkinを持っている時のみリターゲットを行う
		// リターゲットは平行移動成分(Pos)に対して行うものなので、回転だけのアニメーションには必要ない
		if (SourceClip->GetDefaultSkin())
		{
			// RigのReTargetingを行う
			// リターゲティングとはリグの形が異なるアニメーションを自身のアニメーションに合うように調整すること
			// 例えば身長が違うとアバターが伸びてしまう
			if (!ReTargetRig(SourceClip, TargetClip)) return;
		}

		TargetClip->SetIsLoop(IsLoop);

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

			for (const auto& Skin : m_SkinList)
			{
				for (const auto& Joint : Skin->GetJointList())
				{
					const auto& Node = Joint->GetJointNode();

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
		const auto& SourceSkin = SourceClip->GetDefaultSkin();
		if (!SourceSkin) return false;

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

			const auto& SourceBone = SourceSkin->GetBone(BoneName);
			if (!SourceBone) continue;

			const glm::mat4 SourceRestMove = SourceBone->GetJointNode()->GetDefaultLocalMoveMatrix();
			const glm::mat4 InverseSourceRestMove = glm::inverse(SourceRestMove);

			for (const auto& TargetSkin : m_SkinList)
			{
				const auto& TargetBone = TargetSkin->GetBone(BoneName);
				if (!TargetBone) continue;

				const glm::mat4 TargetRestMove = TargetBone->GetJointNode()->GetDefaultLocalMoveMatrix();

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

				// 対象のBoneについては一度しか計算しない
				break;
			}
		}

		return true;
	}
}

#endif