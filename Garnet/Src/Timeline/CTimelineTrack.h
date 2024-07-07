#pragma once

#include <vector>
#include <memory>

#include "CTimelineTrackContent.h"
#include "ETimelineSamplerTarget.h"

namespace timeline
{
	class CTimelineTrack
	{
		std::string m_TrackID;

		int m_SamplerIndex;

		const ETimelineSamplerTarget m_SamplerTarget;
	public:
		CTimelineTrack(const std::string& TrackID, int SamplerIndex, ETimelineSamplerTarget SamplerTarget);
		virtual ~CTimelineTrack() = default;

		virtual int GetSamplerIndex() const;

		virtual ETimelineSamplerTarget GetSamplerTarget() const;

		virtual void SetTrackID(const std::string& TrackID);
		virtual const std::string& GetTrackID() const;

		virtual bool Update(float CurrentTime, const std::vector<float>& Value);

		virtual void AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent);

		static std::string GenerateUUID();
	};
}