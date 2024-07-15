#pragma once

#include <memory>
#include <vector>

#include "CTimelineClip.h"

namespace object { class C3DObject; }

namespace timeline
{
	class CTimelineController
	{
		float m_PlayBackTime;
		
		std::shared_ptr<CTimelineClip> m_Clip;

		bool m_Play;

	private:
		bool UpdateClip(float CurrentTime);

	public:
		CTimelineController();
		CTimelineController(float CurrentTime, const std::shared_ptr<CTimelineClip>& Clip, bool PlayFlag);
		virtual ~CTimelineController();
		
		void SetPlayBackTime(float Time);
		float GetPlayBackTime() const;

		void SetMaxTime(float Time);
		float GetMaxTime() const;

		void SetClip(const std::shared_ptr<CTimelineClip>& Clip);
		const std::shared_ptr<CTimelineClip>& GetClip() const;

		void SetPlay(bool Flag);
		bool IsPlay() const;

		bool Update(float DeltaSecondsTime);
	};
}