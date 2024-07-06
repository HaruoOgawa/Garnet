#pragma once

#include <memory>
#include <vector>

#include "CTimelineClip.h"

namespace object { class C3DObject; }

namespace timeline
{
	class CTimelineController
	{
		float m_CurrentTime;

		std::shared_ptr<CTimelineClip> m_Clip;

	private:
		bool UpdateClip(float CurrentTime, const std::vector<object::C3DObject>& ObjectList);

	public:
		CTimelineController();
		virtual ~CTimelineController();
	};
}