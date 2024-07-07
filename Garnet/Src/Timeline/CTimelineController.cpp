#include "CTimelineController.h"

namespace timeline
{
	CTimelineController::CTimelineController(float CurrentTime, const std::shared_ptr<CTimelineClip>& Clip, bool PlayFlag) :
		m_CurrentTime(CurrentTime),
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
		if (m_CurrentTime < 0.0f || m_CurrentTime > GetMaxTime()) return true;

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