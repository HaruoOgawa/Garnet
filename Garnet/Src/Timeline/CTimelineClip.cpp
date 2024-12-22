#include "CTimelineClip.h"
#include "../Object/C3DObject.h"
#include "../Scene/CSceneController.h"

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

	std::shared_ptr<CTimelineTrack> CTimelineClip::FindTrack(const std::string& Key) const
	{
		const auto it = m_TrackList.find(Key);
		if (it == m_TrackList.end()) return nullptr;

		return it->second;
	}

	void CTimelineClip::AssignObjectResourceToTrack(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, const std::shared_ptr<scene::CSceneController>& SceneController)
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
			for (const auto& Mesh : Object->GetMeshList())
			{
				for (const auto& Primitive : Mesh->GetPrimitiveList())
				{
					for (const auto& Renderer : Primitive->GetRendererList())
					{
						const auto& Material = std::get<1>(Renderer);
						if (!Material) continue;

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
		
		// SceneControllerのリソースをトラックに割り当てる
		if (SceneController)
		{
			for (const auto& ValueRegistry : SceneController->GetValueRegistryList())
			{
				for (const auto& RefTrackID : ValueRegistry.second->GetRefTrackIDList())
				{
					const auto& it = m_TrackList.find(RefTrackID);
					if (it == m_TrackList.end())
					{
						// トラックが存在しなかったら削除する
						ValueRegistry.second->RemoveRefTrackID(RefTrackID);

						continue;
					}

					it->second->AssignTrackContent(ValueRegistry.second);
				}

				ValueRegistry.second->ShrinkToFitTrackIDList();
			}
		}
	}

	void CTimelineClip::RemoveTrackAndSampler(const std::string& TrackID)
	{
		auto it = m_TrackList.find(TrackID);

		if (it == m_TrackList.end()) return;

		const int RemovedSamplerIndex = it->second->GetSamplerIndex();

		// Trackを削除
		m_TrackList.erase(it);

		// Samplerを削除する
		if (RemovedSamplerIndex >= 0 && RemovedSamplerIndex < static_cast<int>(m_SamplerList.size()))
		{
			m_SamplerList.erase(m_SamplerList.begin() + RemovedSamplerIndex);
			m_SamplerList.shrink_to_fit();

			// SamplerIndexを詰める
			for (auto& Track : m_TrackList)
			{
				int ExistSamplerIndex = Track.second->GetSamplerIndex();

				// 消されたサンプラーよりも後ろのインデックスを示していたら１だけインデックスを詰める
				if (ExistSamplerIndex <= RemovedSamplerIndex) continue;

				Track.second->SetSamplerIndex(ExistSamplerIndex - 1);
			}
		}
	}
}