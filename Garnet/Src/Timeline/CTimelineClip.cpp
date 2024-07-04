#include "CTimelineClip.h"

namespace timeline
{
	CTimelineClip::CTimelineClip()
	{
	}

	CTimelineClip::~CTimelineClip()
	{
	}

	bool CTimelineClip::Update(float CurrentTime)
	{
		for (const auto& Track : m_TrackList)
		{
			int SamplerIndex = Track->GetSamplerIndex();
			if (SamplerIndex < 0 || SamplerIndex >= m_SamplerList.size()) continue;

			//
			animation::EInterpolateValueType ValueType = animation::EInterpolateValueType::NONE;
			/*if (Channel->GetAnimationTarget() == animation::EAnimationTarget::ROTATION)
			{
				ValueType = animation::EInterpolateValueType::QUATERNION;
			}
			else if (Channel->GetAnimationTarget() == animation::EAnimationTarget::MODELMATRIX)
			{
				ValueType = animation::EInterpolateValueType::MODELMATRIX;
			}*/

			// サンプラーと再生時間から現在のキーフレームの値を取得
			const auto& Sampler = m_SamplerList[SamplerIndex];
			std::vector<float> Value;

			if (!Sampler->ComputeCurrentFrame(CurrentTime, false, Value, ValueType)) return false;
		}

		return true;
	}
}