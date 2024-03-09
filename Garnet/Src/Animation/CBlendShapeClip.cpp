#include "CBlendShapeClip.h"

#ifdef USE_ANIMATION
namespace animation
{
	void CBlendShapeClip::AddAnimationChannel(EBlendShapeName BlendShapeName, const std::shared_ptr<animation::CAnimationChannel>& AnimationChannel)
	{
		m_BlendShapeNameList.push_back(BlendShapeName);
		m_ChannelList.push_back(AnimationChannel);
	}

	const std::vector<EBlendShapeName>& CBlendShapeClip::GetBlendShapeNameList() const
	{
		return m_BlendShapeNameList;
	}

	const std::vector<float>& CBlendShapeClip::GetCurrentMorphWeights() const
	{
		return m_CurrentMorphWeights;
	}

	bool CBlendShapeClip::UpdateFrame(float CurrentTime)
	{
		m_CurrentMorphWeights.clear();

		for (const auto& Channel : m_ChannelList)
		{
			int SamplerIndex = Channel->GetSamplerIndex();
			if (SamplerIndex < 0 || SamplerIndex >= m_SamplerList.size()) continue;

			const auto& Sampler = m_SamplerList[SamplerIndex];
			std::vector<float> Value;

			if (!Sampler->ComputeCurrentFrame(CurrentTime, m_IsLoop, Value, Channel->GetAnimationTarget())) return false;

			for (float v : Value)
			{
				// Weightは0から1の値で小数点2位までにする
				// あまり細かいと頻繁にモーフを更新することになるため
				float Weight = fmaxf(0.0f, fminf(1.0f, v));

				Weight = floorf(Weight * 100.0f) / 100.0f;

				m_CurrentMorphWeights.push_back(Weight);
			}
		}

		return true;
	}
}
#endif