#include "CTimelineController.h"

namespace timeline
{
	CTimelineController::CTimelineController(float CurrentTime, float MaxTime, const std::shared_ptr<CTimelineClip>& Clip, bool PlayFlag) :
		m_CurrentTime(CurrentTime),
		m_MaxTime(MaxTime),
		m_Clip(Clip),
		m_Play(PlayFlag)
	{
	}

	CTimelineController::CTimelineController():
		CTimelineController(0.0f, 0.0f, nullptr, false)
	{
	}

	CTimelineController::~CTimelineController()
	{
	}

	void CTimelineController::SetCurrentTime(float Time)
	{
		m_CurrentTime = Time;
	}

	float CTimelineController::GetCurrentTime() const
	{
		return m_CurrentTime;
	}

	void CTimelineController::SetMaxTime(float Time)
	{
		m_MaxTime = Time;
	}

	float CTimelineController::GetMaxTime() const
	{
		return m_MaxTime;
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
		if (m_CurrentTime < 0.0f || m_CurrentTime > m_MaxTime) return true;

		if (m_Play)
		{
			m_CurrentTime += DeltaSecondsTime;
		}

		if (!UpdateClip(m_CurrentTime)) return false;

		return true;
	}

	bool CTimelineController::UpdateClip(float CurrentTime)
	{
		if (!m_Clip) return true;

		if (!m_Clip->Update(CurrentTime)) return false;

		return true;
	}
}