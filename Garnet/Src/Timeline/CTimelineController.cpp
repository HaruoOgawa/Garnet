#include "CTimelineController.h"

namespace timeline
{
	CTimelineController::CTimelineController(float CurrentTime, const std::shared_ptr<CTimelineClip>& Clip, bool PlayFlag) :
		m_PlayBackTime(CurrentTime),
		m_Clip(Clip),
		m_Play(PlayFlag)
	{
	}

	CTimelineController::CTimelineController():
		CTimelineController(0.0f, nullptr, false)
	{
	}

	CTimelineController::~CTimelineController()
	{
	}

	void CTimelineController::SetPlayBackTime(float Time)
	{
		m_PlayBackTime = Time;
	}

	float CTimelineController::GetPlayBackTime() const
	{
		return m_PlayBackTime;
	}

	void CTimelineController::SetMaxTime(float Time)
	{
		if (m_Clip) m_Clip->SetMaxTime(Time);
	}

	float CTimelineController::GetMaxTime() const
	{
		if (!m_Clip) return 0.0f;

		return m_Clip->GetMaxTime();
	}

	void CTimelineController::SetClip(const std::shared_ptr<CTimelineClip>& Clip)
	{
		m_Clip = Clip;
	}

	const std::shared_ptr<CTimelineClip>& CTimelineController::GetClip() const
	{
		return m_Clip;
	}

	void CTimelineController::SetPlay(bool Flag)
	{
		m_Play = Flag;
	}

	bool CTimelineController::IsPlay() const
	{
		return m_Play;
	}

	bool CTimelineController::Update(float DeltaSecondsTime)
	{
		if (m_PlayBackTime < 0.0f || m_PlayBackTime > GetMaxTime()) return true;

		if (m_Play)
		{
			m_PlayBackTime += DeltaSecondsTime;
		}

		if (!UpdateClip(m_PlayBackTime)) return false;

		return true;
	}

	bool CTimelineController::UpdateClip(float CurrentTime)
	{
		if (!m_Clip) return true;

		if (!m_Clip->Update(CurrentTime)) return false;

		return true;
	}
}