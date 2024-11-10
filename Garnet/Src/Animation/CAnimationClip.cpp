#ifdef USE_ANIMATION

#include "CAnimationClip.h"
#include "CSkeleton.h"

namespace animation
{
	CAnimationClip::CAnimationClip():
		m_CurrentTime(0.0f),
		m_ClipStartTime(0.0f),
		m_ClipEndTime(0.0f),
		m_IsLoop(false),
		m_UseIK(false),
		m_DefaultSkeleton(nullptr)
	{
	}

	CAnimationClip::~CAnimationClip()
	{
	}

	void CAnimationClip::SetIsLoop(bool val)
	{
		m_IsLoop = val;
	}

	bool CAnimationClip::IsLoop() const
	{
		return m_IsLoop;
	}

	void CAnimationClip::SetUseIK(bool Val)
	{
		m_UseIK = Val;
	}

	bool CAnimationClip::IsUseIK() const
	{
		return m_UseIK;
	}

	void CAnimationClip::Initialize()
	{
		m_CurrentTime = 0.0f;
	}

	void CAnimationClip::AddAnimationSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler)
	{
		Sampler->SetSelfSamplerIndex(static_cast<int>(m_SamplerList.size()));

		m_SamplerList.push_back(Sampler);
	}

	void CAnimationClip::AddAnimationChannel(const std::shared_ptr<animation::CAnimationChannel>& AnimationChannel)
	{
		m_ChannelList.push_back(AnimationChannel);
	}

	void CAnimationClip::SetDefaultSkeleton(const std::shared_ptr<animation::CSkeleton>& DefaultSkeleton)
	{
		m_DefaultSkeleton = DefaultSkeleton;
	}

	const std::shared_ptr<animation::CSkeleton>& CAnimationClip::GetDefaultSkeleton() const
	{
		return m_DefaultSkeleton;
	}

	const std::vector<std::shared_ptr<animation::CAnimationSampler>>& CAnimationClip::GetSamplerList() const
	{
		return m_SamplerList;
	}

	const std::vector<std::shared_ptr<animation::CAnimationChannel>>& CAnimationClip::GetChannelList() const
	{
		return m_ChannelList;
	}

	bool CAnimationClip::Update(float DeltaSecondsTime, const std::shared_ptr<CSkeleton>& Skeleton)
	{
		m_CurrentTime += DeltaSecondsTime;

		if (m_IsLoop)
		{
			m_CurrentTime = glm::mod(m_CurrentTime, m_ClipEndTime);
		}
		else
		{
			m_CurrentTime = fminf(m_CurrentTime, m_ClipEndTime);
		}

		if (!UpdateFrame(m_CurrentTime, Skeleton)) return false;

		return true;
	}

	bool CAnimationClip::UpdateFrame(float CurrentTime, const std::shared_ptr<CSkeleton>& Skeleton)
	{
		for (const auto& Channel : m_ChannelList)
		{
			int SamplerIndex = Channel->GetSamplerIndex();
			if (SamplerIndex < 0 || SamplerIndex >= m_SamplerList.size()) continue;

			//
			EInterpolateValueType ValueType = EInterpolateValueType::NONE;
			if (Channel->GetAnimationTarget() == EAnimationTarget::ROTATION)
			{
				ValueType = EInterpolateValueType::QUATERNION;
			}
			else if (Channel->GetAnimationTarget() == EAnimationTarget::MODELMATRIX)
			{
				ValueType = EInterpolateValueType::MODELMATRIX;
			}

			//
			const auto& Sampler = m_SamplerList[SamplerIndex];
			std::vector<float> Value;

			if (!Sampler->ComputeCurrentFrame(CurrentTime, m_IsLoop, Value, ValueType)) return false;

			// 空でも動くように初期値を渡す
			if(Value.empty()) Value = GetDefaultValueFromAnimationTarget(Channel->GetAnimationTarget());
			
			// ノードを取得
			std::shared_ptr<object::CNode> TargetNode = FindTargetNode(Skeleton, Channel);
			
			//
			if (!Channel->Update(Value, TargetNode)) return false;
		}

		return true;
	}

	std::shared_ptr<object::CNode> CAnimationClip::FindTargetNode(const std::shared_ptr<CSkeleton>& Skeleton, const std::shared_ptr<animation::CAnimationChannel>& Channel)
	{
		// DefaultSkeletonのリグを見ることでヒューマノイドでも通常のアニメーションクリップが再生できるようになる
		animation::ERigType RigType = (m_DefaultSkeleton) ? m_DefaultSkeleton->GetRig() : Skeleton->GetRig();

		// ヒューマノイドボーン
		if (RigType == ERigType::Humanoid)
		{
			const auto& BoneTable = Skeleton->GetHumanoidBoneTable();
			const auto& it = BoneTable.find(Channel->GetBoneName());

			if (it != BoneTable.end() && it->second) return it->second->GetBoneNode();
		}

		// 非ヒューマノイドボーン(普通のスキンメッシュアニメーション)
		// ヒューマノイドボーンが見つからなかったらこっちから探す
		{
			const auto& BoneList = Skeleton->GetBoneList();
			const auto it = std::find_if(BoneList.begin(), BoneList.end(), [&](const auto& Src) { return (std::get<0>(Src) == Channel->GetTargetNodeName()); });

			if (it != BoneList.end()) return std::get<1>(*it)->GetBoneNode();
		}

		return nullptr;
	}

	int CAnimationClip::GetFrameCount()
	{
		int Count = 0;

		if (m_SamplerList.size() > 0)
		{
			Count = static_cast<int>(m_SamplerList[0]->GetKeyFrameList().size());
		}

		return Count;
	}

	// 一番フレーム数が多いSamplerを取得する
	std::shared_ptr<animation::CAnimationSampler> CAnimationClip::GetSamplerWithMostFrames()
	{
		std::shared_ptr<animation::CAnimationSampler> Result = nullptr;

		size_t maxSize = 0;

		for (const auto& Sampler : m_SamplerList)
		{
			size_t size = Sampler->GetKeyFrameList().size();

			if (size > maxSize)
			{
				size = maxSize;

				Result = Sampler;
			}
		}

		return Result;
	}

	float CAnimationClip::GetCurrentTime() const
	{
		return m_CurrentTime;
	}

	void CAnimationClip::CalculateClipStartEnd()
	{
		for (const auto& Sampler : m_SamplerList)
		{
			m_ClipStartTime = glm::min(m_ClipStartTime, Sampler->GetStartTime());
			m_ClipEndTime = glm::max(m_ClipEndTime, Sampler->GetEndTime());
		}
	}

	bool CAnimationClip::IsEnd()
	{
		if (m_SamplerList.size() > 0)
		{
			return m_SamplerList[0]->IsEnd(m_CurrentTime);
		}

		return true;
	}

	std::vector<float> CAnimationClip::GetDefaultValueFromAnimationTarget(EAnimationTarget AnimationTarget)
	{
		std::vector<float> Value;

		switch (AnimationTarget)
		{
		case animation::EAnimationTarget::NONE:
			break;
		case animation::EAnimationTarget::TRANSLATION:
			Value = std::vector<float>({ 0.0f, 0.0f, 0.0f });
			break;
		case animation::EAnimationTarget::ROTATION:
			Value = std::vector<float>({ 0.0f, 0.0f, 0.0f, 1.0f });
			break;
		case animation::EAnimationTarget::SCALE:
			Value = std::vector<float>({ 1.0f, 1.0f, 1.0f });
			break;
		case animation::EAnimationTarget::WEIGHTS:
			Value = std::vector<float>({ 0.0f, 0.0f, 0.0f, 0.0f });
			break;
		case animation::EAnimationTarget::MODELMATRIX:
		{
			Value = std::vector<float>({
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				});
		}
		break;
		default:
			break;
		}

		return Value;
	}
}

#endif // USE_ANIMATION