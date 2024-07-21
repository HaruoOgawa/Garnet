#include "CTimelineClip.h"
#include "../Object/C3DObject.h"

namespace timeline
{
	CTimelineClip::CTimelineClip():
		m_FileName(std::string()),
		m_MaxTime(0.0f)
	{
	}

	CTimelineClip::~CTimelineClip()
	{
	}

	void CTimelineClip::SetFileName(const std::string& Name)
	{
		m_FileName = Name;
	}

	const std::string& CTimelineClip::GetFileName() const
	{
		return m_FileName;
	}

	void CTimelineClip::SetMaxTime(float Time)
	{
		m_MaxTime = Time;
	}

	float CTimelineClip::GetMaxTime() const
	{
		return m_MaxTime;
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
			if (Track->GetSamplerTarget() == ETimelineSamplerTarget::ROTATION)
			{
				ValueType = animation::EInterpolateValueType::QUATERNION;
			}
			else if (Track->GetSamplerTarget() == ETimelineSamplerTarget::MODELMATRIX)
			{
				ValueType = animation::EInterpolateValueType::MODELMATRIX;
			}

			// サンプラーと再生時間から現在のキーフレームの値を取得
			auto& Sampler = m_SamplerList[SamplerIndex];
			std::vector<float> Value;

			if (!Sampler->ComputeCurrentFrame(CurrentTime, false, Value, ValueType)) return false;

			if (Value.empty()) continue;

			// Trackの更新
			if (!Track->Update(CurrentTime, Value)) return false;
		}

		return true;
	}

	void CTimelineClip::AddSampler(const std::shared_ptr<animation::CAnimationSampler>& Sampler)
	{
		m_SamplerList.push_back(Sampler);
	}

	const std::vector<std::shared_ptr<animation::CAnimationSampler>>& CTimelineClip::GetSamplerList() const
	{
		return m_SamplerList;
	}

	void CTimelineClip::AddTrack(const std::shared_ptr<CTimelineTrack>& Track)
	{
		m_TrackList.emplace(Track->GetTrackID(), Track);
	}

	const std::unordered_map<std::string, std::shared_ptr<CTimelineTrack>>& CTimelineClip::GetTrackList() const
	{
		return m_TrackList;
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
					if (it == m_TrackList.end())
					{
						// トラックが存在しなかったら削除する
						Node->RemoveRefTrackID(RefTrackID);

						continue;
					}

					it->second->AssignTrackContent(Node);
				}

				Node->ShrinkToFitTrackIDList();
			}

			// Material
			for (const auto& Material : Object->GetMaterialList())
			{
				for (const auto& RefTrackID : Material->GetRefTrackIDList())
				{
					const auto& it = m_TrackList.find(RefTrackID);
					if (it == m_TrackList.end())
					{
						// トラックが存在しなかったら削除する
						Material->RemoveRefTrackID(RefTrackID);

						continue;
					}

					it->second->AssignTrackContent(Material);
				}

				Material->ShrinkToFitTrackIDList();
			}
		}
		
	}
}