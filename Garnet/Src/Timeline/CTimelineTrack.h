#pragma once

#include <vector>
#include <memory>

#include "STimelineConnector.h"
#include "CTimelineTrackContent.h"
#include "../Animation/EAnimationTarget.h"

namespace timeline
{
	class CTimelineTrack
	{
		std::string m_TrackID;

		int m_SamplerIndex;

		const animation::EAnimationTarget m_AnimationTarget;
	public:
		CTimelineTrack(const std::string& TrackID, int SamplerIndex, animation::EAnimationTarget AnimationTarget);
		virtual ~CTimelineTrack() = default;

		virtual int GetSamplerIndex() const;

		virtual animation::EAnimationTarget GetAnimationTarget() const;

		virtual void SetTrackID(const std::string& TrackID);
		virtual const std::string& GetTrackID() const;

		virtual bool Update(float CurrentTime, const std::vector<float>& Value);

		virtual void AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent);

		static std::string GenerateUUID();
	};
}