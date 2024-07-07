#pragma once

#include <memory>
#include "CTimelineTrack.h"

namespace object { class CNode; }

namespace timeline
{
	enum class ENodeTrackTarget
	{
		NodeTrackTarget_None = -1,

		NodeTrackTarget_EnabledFlag,
		NodeTrackTarget_Translation,
		NodeTrackTarget_Rotation,
		NodeTrackTarget_Scale,
	};

	class CNodeTrack : public CTimelineTrack
	{
		const ENodeTrackTarget m_TrackTarget;

		std::shared_ptr<object::CNode> m_Node;

	private:
		bool UpdateEnabledFlag(const std::vector<float>& Value);
		bool UpdateTranslation(const std::vector<float>& Value);
		bool UpdateRotation(const std::vector<float>& Value);
		bool UpdateScale(const std::vector<float>& Value);

	public:
		CNodeTrack(const std::string& TrackID, int SamplerIndex, ETimelineSamplerTarget SamplerTarget, ENodeTrackTarget TrackTarget);
		virtual ~CNodeTrack();

		virtual bool Update(float CurrentTime, const std::vector<float>& Value) override;

		virtual void AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent) override;
	};
}