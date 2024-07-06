#pragma once

#include <memory>
#include <string>
#include "CTimelineTrack.h"
#include "../Animation/EKeyFrameType.h"

namespace graphics { class CMaterial; }

namespace timeline
{
	enum class EMaterialTrackTarget
	{
		MaterialTrackTarget_None = -1,

		MaterialTrackTarget_SetUniformValue,
	};

	class CMaterialTrack : public CTimelineTrack
	{
		const EMaterialTrackTarget m_TrackTarget;

		std::shared_ptr<graphics::CMaterial> m_Material;

		std::string m_UniformName;
		animation::EKeyFrameType m_ValueType;

	private:
		int GetByteSizeFromValueType(animation::EKeyFrameType Type);
	public:
		CMaterialTrack(const std::string& TrackID, int SamplerIndex, animation::EAnimationTarget AnimationTarget, EMaterialTrackTarget TrackTarget, 
			const std::string& UniformName, animation::EKeyFrameType ValueType);
		virtual ~CMaterialTrack();

		virtual bool Update(float CurrentTime, const std::vector<float>& Value) override;

		virtual void AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent) override;
	};
}