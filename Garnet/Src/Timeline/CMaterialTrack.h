#pragma once

#include <memory>
#include "CTimelineTrack.h"

namespace object { class C3DObject; }

namespace timeline
{
	class CMaterialTrack : public CTimelineTrack
	{
	public:
		CMaterialTrack(int SamplerIndex, animation::EAnimationTarget AnimationTarget, const STimelineConnector& Connector);
		virtual ~CMaterialTrack();

		virtual bool Update(float CurrentTime, const std::vector<float>& Value, const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList) override;
	};
}