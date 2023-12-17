#ifdef USE_ANIMATION

#include "CAnimationController.h"
#include "../Object/CNode.h"

namespace animation
{
	CAnimationController::CAnimationController():
		m_MaxBlendingTime(0.5f),
		m_CurrBlendingTime(0.0f),
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

			std::shared_ptr<animation::CAnimationChannel> TargetChannel = std::make_shared<animation::CAnimationChannel>(SourceChannel->GetSamplerIndex(), SourceChannel->GetAnimationTarget(), TargetNode, SourceChannel->GetBoneName());

			TargetClip->AddAnimationChannel(TargetChannel);
		}

		// RigのReTargetingを行う
		// リターゲティングとはリグの形が異なるアニメーションを自身のアニメーションに合うように調整すること
		// 例えば身長が違うとアバターが伸びてしまうしリグが反対だとねじれてしまう
		//if (!ReTargetingRig(SourceClip, TargetClip)) return;

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
		return true;
	}

	bool CAnimationController::ReTargetingRig(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::shared_ptr<animation::CAnimationClip>& TargetClip)
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

			// BoneTableに登録されていないものについては処理の対象外とする
			if (BoneName == animation::EHumanoidBones::None) continue;

			const auto& SourceBone = SourceSkin->GetBone(BoneName);
			if (!SourceBone) continue;

			const glm::mat4 SourceRest = SourceBone->GetJointNode()->GetDefaultLocalMatrix();
			const glm::mat4 InverseSourceRest = glm::inverse(SourceRest);

			const glm::mat4 SourcePGRest = SourceBone->GetJointNode()->CalcDefaultParentWorldMatrix();
			const glm::mat4 InverseSourcePGRest = glm::inverse(SourcePGRest);

			for (const auto& TargetSkin : m_SkinList)
			{
				const auto& TargetBone = TargetSkin->GetBone(BoneName);
				if (!TargetBone) continue;

				const glm::mat4 TargetRest = TargetBone->GetJointNode()->GetDefaultLocalMatrix();

				const glm::mat4 TargetPGRest = TargetBone->GetJointNode()->CalcDefaultParentWorldMatrix();
				const glm::mat4 InverseTargetPGRest = glm::inverse(TargetPGRest);

				for (const auto& TargetKeyFrame : TargetSampler->GetKeyFrameList())
				{
					const float CurrentTime = TargetKeyFrame->GetInput();

					glm::mat4 SourcePose = glm::mat4(1.0f);
					TargetKeyFrame->GetOutput(&SourcePose[0][0]);

					glm::mat4 SourceAnim = SourcePGRest * SourcePose * InverseSourceRest * InverseSourcePGRest;

					glm::mat4 TargetPose = InverseTargetPGRest * SourceAnim * TargetPGRest * TargetRest;

					TargetKeyFrame->SetOutput(&TargetPose[0][0], sizeof(glm::mat4));
				}

				// 対象のBoneについては一度しか計算しない
				break;
			}
		}

		return true;
	}
}

#endif