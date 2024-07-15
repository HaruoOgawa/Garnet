#include "CMaterialTrack.h"
#include "../Graphics/CMaterial.h"

namespace timeline
{
	CMaterialTrack::CMaterialTrack(const std::string& TrackID, int SamplerIndex, ETimelineSamplerTarget SamplerTarget, EMaterialTrackTarget TrackTarget,
		const std::string& UniformName, math::EValueType ValueType):
		CTimelineTrack(TrackID, SamplerIndex, SamplerTarget),
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
}