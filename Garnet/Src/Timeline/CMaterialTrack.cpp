#include "CMaterialTrack.h"
#include "../Graphics/CMaterial.h"

namespace timeline
{
	CMaterialTrack::CMaterialTrack(const std::string& TrackID, int SamplerIndex, ETimelineSamplerTarget SamplerTarget, EMaterialTrackTarget TrackTarget,
		const std::string& UniformName, math::EValueType ValueType):
		CTimelineTrack(TrackID, SamplerIndex, SamplerTarget, ETrackType::TrackType_Material),
		m_TrackTarget(TrackTarget),
		m_Material(nullptr),
		m_UniformName(UniformName)
	{
		SetValueType(ValueType);
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
			m_Material->SetUniformValue(m_UniformName, &Value[0], math::CMath::GetByteSizeFromValueType(GetValueType()));
			break;

		default:
			break;
		}

		return true;
	}

	void CMaterialTrack::AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent)
	{
		m_Material = std::dynamic_pointer_cast<graphics::CMaterial>(TrackContent);
	}

	std::string CMaterialTrack::GetTrackName()
	{
		std::string Name = std::string();

		switch (m_TrackTarget)
		{
		case EMaterialTrackTarget::MaterialTrackTarget_None:
			break;

		case EMaterialTrackTarget::MaterialTrackTarget_SetUniformValue:
			Name = m_UniformName + " (Uniform)";
			break;
		default:
			break;
		}

		return Name;
	}

	int CMaterialTrack::GetParam_Int(const std::string Name)
	{
		if (Name == "TrackTarget")
		{
			return static_cast<int>(m_TrackTarget);
		}

		return -1;
	}

	float CMaterialTrack::GetParam_Float(const std::string Name)
	{
		return 0.0f;
	}

	std::string CMaterialTrack::GetParam_String(const std::string Name)
	{
		if (Name == "UniformName")
		{
			return m_UniformName;
		}

		return std::string();
	}

	std::string CMaterialTrack::CastMaterialTrackTarget_Str(EMaterialTrackTarget TrackTarget)
	{
		std::string dst = std::string();

		switch (TrackTarget)
		{
		case timeline::EMaterialTrackTarget::MaterialTrackTarget_None:
			dst = "None";
			break;
		case timeline::EMaterialTrackTarget::MaterialTrackTarget_SetUniformValue:
			dst = "SetUniform";
			break;
		case timeline::EMaterialTrackTarget::MaterialTrackTarget_Max:
			dst = "Max";
			break;
		default:
			break;
		}

		return dst;
	}
}