#pragma once

#include <memory>
#include <string>
#include "CTimelineTrack.h"

namespace scriptable { class CValueRegistry; }

namespace timeline
{
	class CCustomTrack : public CTimelineTrack
	{
		std::shared_ptr<scriptable::CValueRegistry> m_TargetValueRegistry;

		const std::string m_TargetValueName;
	public:
		CCustomTrack(const std::string& TrackID, int SamplerIndex, ETimelineSamplerTarget SamplerTarget, const std::string& TargetValueName);
		virtual ~CCustomTrack();

		virtual bool Update(float CurrentTime, const std::vector<float>& Value) override;

		virtual void AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent) override;

		virtual std::string GetTrackName() override;

		virtual int GetParam_Int(const std::string Name) override;
		virtual float GetParam_Float(const std::string Name) override;
		virtual std::string GetParam_String(const std::string Name) override;
	};
}