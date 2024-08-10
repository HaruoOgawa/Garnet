#include "CTimelineTrack.h"
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>

namespace timeline
{
	CTimelineTrack::CTimelineTrack(const std::string& TrackID, int SamplerIndex, ETimelineSamplerTarget SamplerTarget, ETrackType TrackType):
		m_TrackID(TrackID),
		m_SamplerIndex(SamplerIndex),
        m_SamplerTarget(SamplerTarget),
        m_ValueType(math::EValueType::VALUE_TYPE_NONE),
        m_TrackType(TrackType)
	{
	}

    void CTimelineTrack::SetSamplerIndex(int Index)
    {
        m_SamplerIndex = Index;
    }

	int CTimelineTrack::GetSamplerIndex() const
	{
		return m_SamplerIndex;
	}

    ETimelineSamplerTarget CTimelineTrack::GetSamplerTarget() const
	{
		return m_SamplerTarget;
	}

    void CTimelineTrack::SetTrackID(const std::string& TrackID)
    {
        m_TrackID = TrackID;
    }

    const std::string& CTimelineTrack::GetTrackID() const
    {
        return m_TrackID;
    }

    ETrackType CTimelineTrack::GetTrackType() const
    {
        return m_TrackType;
    }

	bool CTimelineTrack::Update(float CurrentTime, const std::vector<float>& Value)
	{
		return true;
	}

    void CTimelineTrack::AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent)
    {
    }

    std::string CTimelineTrack::GetTrackName()
    {
        return std::string();
    }

    int CTimelineTrack::GetParam_Int(const std::string Name)
    {
        return -1;
    }

    float CTimelineTrack::GetParam_Float(const std::string Name)
    {
        return 0.0f;
    }

    std::string CTimelineTrack::GetParam_String(const std::string Name)
    {
        return std::string();
    }

    void CTimelineTrack::SetValueType(math::EValueType ValueType)
    {
        m_ValueType = ValueType;
    }
    
    math::EValueType CTimelineTrack::GetValueType() const
    {
        return m_ValueType;
    }

	std::string CTimelineTrack::GenerateUUID()
	{
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        std::uniform_int_distribution<> dis2(8, 11);

        std::stringstream ss;
        ss << std::hex;
        ss << std::nouppercase;

        for (int i = 0; i < 8; ++i) {
            ss << dis(gen);
        }
        ss << "-";
        for (int i = 0; i < 4; ++i) {
            ss << dis(gen);
        }
        ss << "-4"; // 4 signifies the UUID version 4
        for (int i = 0; i < 3; ++i) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen); // ensures the first character is in the range [8, b]
        for (int i = 0; i < 3; ++i) {
            ss << dis(gen);
        }
        ss << "-";
        for (int i = 0; i < 12; ++i) {
            ss << dis(gen);
        }
        return ss.str();
	}
}