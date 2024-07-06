#include "CTimelineTrack.h"
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>

namespace timeline
{
	CTimelineTrack::CTimelineTrack(const std::string& TrackID, int SamplerIndex, animation::EAnimationTarget AnimationTarget):
		m_TrackID(TrackID),
		m_SamplerIndex(SamplerIndex),
		m_AnimationTarget(AnimationTarget)
	{
	}

	int CTimelineTrack::GetSamplerIndex() const
	{
		return m_SamplerIndex;
	}

	animation::EAnimationTarget CTimelineTrack::GetAnimationTarget() const
	{
		return m_AnimationTarget;
	}

    void CTimelineTrack::SetTrackID(const std::string& TrackID)
    {
        m_TrackID = TrackID;
    }

    const std::string& CTimelineTrack::GetTrackID() const
    {
        return m_TrackID;
    }

	bool CTimelineTrack::Update(float CurrentTime, const std::vector<float>& Value)
	{
		return true;
	}

    void CTimelineTrack::AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent)
    {
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