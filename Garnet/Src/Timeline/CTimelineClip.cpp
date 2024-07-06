#include "CTimelineClip.h"

namespace timeline
{
	CTimelineClip::CTimelineClip()
	{
	}

	CTimelineClip::~CTimelineClip()
	{
	}

	bool CTimelineClip::Update(float CurrentTime, const std::vector<object::C3DObject>& ObjectList)
	{
		for (auto& Track : m_TrackList)
		{
			int SamplerIndex = Track->GetSamplerIndex();
			if (SamplerIndex < 0 || SamplerIndex >= m_SamplerList.size()) continue;

			//
			animation::EInterpolateValueType ValueType = animation::EInterpolateValueType::NONE;
			if (Track->GetAnimationTarget() == animation::EAnimationTarget::ROTATION)
			{
				ValueType = animation::EInterpolateValueType::QUATERNION;
			}
			else if (Track->GetAnimationTarget() == animation::EAnimationTarget::MODELMATRIX)
			{
				ValueType = animation::EInterpolateValueType::MODELMATRIX;
			}

			// サンプラーと再生時間から現在のキーフレームの値を取得
			auto& Sampler = m_SamplerList[SamplerIndex];
			std::vector<float> Value;

			if (!Sampler->ComputeCurrentFrame(CurrentTime, false, Value, ValueType)) return false;

			// Trackの更新
			if (!Track->Update(CurrentTime, Value, ObjectList)) return false;
		}

		return true;
	}

	void CTimelineClip::AddSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler)
	{
		m_SamplerList.push_back(Sampler);
	}

	void CTimelineClip::AddTrack(const std::shared_ptr<CTimelineTrack>& Track)
	{
		m_TrackList.push_back(Track);
	}
}