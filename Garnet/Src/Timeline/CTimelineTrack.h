#pragma once

#include <vector>
#include <memory>

#include "CTimelineTrackContent.h"
#include "ETimelineSamplerTarget.h"
#include "../Math/CMath.h"

namespace timeline
{
	enum class ETrackType
	{
		TrackType_None = -1,

		TrackType_Node,
		TrackType_Material,
	};

	class CTimelineTrack
	{
		std::string m_TrackID;

		int m_SamplerIndex;

		const ETimelineSamplerTarget m_SamplerTarget;

		math::EValueType m_ValueType;

		const ETrackType m_TrackType;
	public:
		CTimelineTrack(const std::string& TrackID, int SamplerIndex, ETimelineSamplerTarget SamplerTarget, ETrackType TrackType);
		virtual ~CTimelineTrack() = default;

		virtual int GetSamplerIndex() const;

		virtual ETimelineSamplerTarget GetSamplerTarget() const;

		virtual void SetTrackID(const std::string& TrackID);
		virtual const std::string& GetTrackID() const;

		virtual ETrackType GetTrackType() const;

		virtual bool Update(float CurrentTime, const std::vector<float>& Value);

		virtual void AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent);

		virtual std::string GetTrackName();

		virtual int GetParam_Int(const std::string Name);
		virtual float GetParam_Float(const std::string Name);
		virtual std::string GetParam_String(const std::string Name);

		virtual void SetValueType(math::EValueType ValueType);
		virtual math::EValueType GetValueType() const;

		static std::string GenerateUUID();
	};
}