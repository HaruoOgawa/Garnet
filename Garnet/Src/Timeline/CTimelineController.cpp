#include "CTimelineController.h"
#ifdef USE_BINARY_WRITE
#include "CTimelineExporter.h"
#endif // USE_BINARY_WRITE
#include "../Input/CInputState.h"

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

	bool CTimelineController::Update(float DeltaSecondsTime, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (m_PlayBackTime < 0.0f || m_PlayBackTime > GetMaxTime()) return true;

		if (m_Play)
		{
			m_PlayBackTime += DeltaSecondsTime;
		}

		if (!UpdateClip(m_PlayBackTime, InputState)) return false;

		return true;
	}

	bool CTimelineController::UpdateClip(float CurrentTime, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_Clip) return true;

		if (!m_Clip->Update(CurrentTime)) return false;

#ifdef USE_BINARY_WRITE
		// タイムラインクリップの書き出し
		if (InputState->IsKeyDown(input::EKeyType::KEY_TYPE_CONTROL) && InputState->IsKeyUp(input::EKeyType::KEY_TYPE_S))
		{
			if (!CTimelineExporter::Export("Resources\\Timeline\\MRTTest.tl", m_Clip)) return false;
			return true;
		}
#endif // USE_BINARY_WRITE

		return true;
	}
}