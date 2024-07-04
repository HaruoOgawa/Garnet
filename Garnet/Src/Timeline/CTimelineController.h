#pragma once

#include <memory>

#include "CTimelineClip.h"

namespace timeline
{
	class CTimelineController
	{
		float m_CurrentTime;

		std::shared_ptr<CTimelineClip> m_Clip;
	public:
		CTimelineController();
		virtual ~CTimelineController();
	};
}