#include "CMaterialTrack.h"
#include "../Graphics/CMaterial.h"

namespace timeline
{
	CMaterialTrack::CMaterialTrack(const std::string& TrackID, int SamplerIndex, animation::EAnimationTarget AnimationTarget, EMaterialTrackTarget TrackTarget,
		const std::string& UniformName, animation::EKeyFrameType ValueType):
		CTimelineTrack(TrackID, SamplerIndex, AnimationTarget),
		m_TrackTarget(TrackTarget),
		m_Material(nullptr),
		m_UniformName(UniformName),
		m_ValueType(ValueType)
	{
	}

	CMaterialTrack::~CMaterialTrack()
	{
	}

	bool CMaterialTrack::Update(float CurrentTime, const std::vector<float>& Value)
	{
		if (!m_Material) return true;

		switch (m_TrackTarget)
		{
		case EMaterialTrackTarget::MaterialTrackTarget_None:
			break;

		case EMaterialTrackTarget::MaterialTrackTarget_SetUniformValue:
			m_Material->SetUniformValue(m_UniformName, &Value[0], GetByteSizeFromValueType(m_ValueType));
			break;

		default:
			break;
		}

		return true;
	}

	int CMaterialTrack::GetByteSizeFromValueType(animation::EKeyFrameType Type)
	{
		if (Type == animation::EKeyFrameType::KEYFRAME_TYPE_SCALAR)
		{
			return 1 * sizeof(float);
		}
		else if (Type == animation::EKeyFrameType::KEYFRAME_TYPE_VEC2)
		{
			return 2 * sizeof(float);
		}
		else if (Type == animation::EKeyFrameType::KEYFRAME_TYPE_VEC3)
		{
			return 3 * sizeof(float);
		}
		else if (Type == animation::EKeyFrameType::KEYFRAME_TYPE_VEC4)
		{
			return 4 * sizeof(float);
		}
		else if (Type == animation::EKeyFrameType::KEYFRAME_TYPE_MAT2)
		{
			return 4 * sizeof(float);
		}
		else if (Type == animation::EKeyFrameType::KEYFRAME_TYPE_MAT3)
		{
			return 9 * sizeof(float);
		}
		else if (Type == animation::EKeyFrameType::KEYFRAME_TYPE_MAT4)
		{
			return 16 * sizeof(float);
		}
		else
		{
			// Unknown component type
			return 0;
		}
	}

	void CMaterialTrack::AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent)
	{
		m_Material = std::dynamic_pointer_cast<graphics::CMaterial>(TrackContent);
	}
}