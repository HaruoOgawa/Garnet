#pragma once

#include <memory>
#include <string>
#include "CTimelineTrack.h"

namespace graphics { class CMaterial; }

namespace timeline
{
	enum class EMaterialTrackTarget
	{
		MaterialTrackTarget_None = -1,

		MaterialTrackTarget_SetUniformValue = 0,

		MaterialTrackTarget_Max = 1,
	};

	class CMaterialTrack : public CTimelineTrack
	{
		const EMaterialTrackTarget m_TrackTarget;

		std::shared_ptr<graphics::CMaterial> m_Material;

		std::string m_UniformName;
		
	public:
		CMaterialTrack(const std::string& TrackID, int SamplerIndex, ETimelineSamplerTarget SamplerTarget, EMaterialTrackTarget TrackTarget,
			const std::string& UniformName, math::EValueType ValueType);
		virtual ~CMaterialTrack();

		virtual bool Update(float CurrentTime, const std::vector<float>& Value) override;

		virtual void AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent) override;

		virtual std::string GetTrackName() override;

		static std::string CastMaterialTrackTarget_Str(EMaterialTrackTarget TrackTarget);
	};
}