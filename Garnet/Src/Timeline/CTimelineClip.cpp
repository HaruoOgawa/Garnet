#include "CTimelineClip.h"
#include "../Object/C3DObject.h"

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
		for (auto& TrackPair : m_TrackList)
		{
			auto& Track = TrackPair.second;

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
			if (!Track->Update(CurrentTime, Value)) return false;
		}

		return true;
	}

	void CTimelineClip::AddSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler)
	{
		m_SamplerList.push_back(Sampler);
	}

	void CTimelineClip::AddTrack(const std::shared_ptr<CTimelineTrack>& Track)
	{
		m_TrackList.emplace(Track->GetTrackID(), Track);
	}

	void CTimelineClip::AssignObjectResourceToTrack(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList)
	{
		// Objectのリソースをトラックに割り当てる
		for (const auto& Object : ObjectList)
		{
			// Node
			for (const auto& Node : Object->GetNodeList())
			{
				for (const auto& RefTrackID : Node->GetRefTrackIDList())
				{
					const auto& it = m_TrackList.find(RefTrackID);
					if (it == m_TrackList.end()) continue;

					it->second->AssignTrackContent(Node);
				}
			}

			// Material
			for (const auto& Material : Object->GetMaterialList())
			{
				for (const auto& RefTrackID : Material->GetRefTrackIDList())
				{
					const auto& it = m_TrackList.find(RefTrackID);
					if (it == m_TrackList.end()) continue;

					it->second->AssignTrackContent(Material);
				}
			}
		}
		
	}
}