#include "CTimelineController.h"

namespace timeline
{
	CTimelineController::CTimelineController():
		m_CurrentTime(0.0f),
		m_Clip(nullptr)
	{
	}

	CTimelineController::~CTimelineController()
	{
	}

	bool CTimelineController::UpdateClip(float CurrentTime, const std::vector<object::C3DObject>& ObjectList)
	{
		if (!m_Clip) return true;

		if (!m_Clip->Update(CurrentTime, ObjectList)) return false;

		return true;
	}
}