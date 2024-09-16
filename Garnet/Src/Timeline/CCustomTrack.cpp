#include "CCustomTrack.h"
#include "../Scriptable/CValueRegistry.h"

namespace timeline
{
	CCustomTrack::CCustomTrack(const std::string& TrackID, int SamplerIndex, ETimelineSamplerTarget SamplerTarget, const std::string& TargetValueName):
		CTimelineTrack(TrackID, SamplerIndex, SamplerTarget, ETrackType::TrackType_Custom),
		m_TargetValueRegistry(nullptr),
		m_TargetValueName(TargetValueName)
	{
	}

	CCustomTrack::~CCustomTrack()
	{
	}

	bool CCustomTrack::Update(float CurrentTime, const std::vector<float>& Value)
	{
		if (!m_TargetValueRegistry) return true;

		m_TargetValueRegistry->SetValue(m_TargetValueName, &Value[0]);

		return true;
	}

	void CCustomTrack::AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent)
	{
		m_TargetValueRegistry = std::dynamic_pointer_cast<scriptable::CValueRegistry>(TrackContent);

		// このタイミングでやっとValueTypeがわかるようになる
		if (m_TargetValueName.empty()) return;

		const auto Value = m_TargetValueRegistry->GetValue(m_TargetValueName);
		graphics::EUniformValueType ValueType = Value.Type;

		switch (ValueType)
		{
		case graphics::EUniformValueType::NONE:
			break;
		case graphics::EUniformValueType::VALUE_TYPE_MAT4:
			SetValueType(math::EValueType::VALUE_TYPE_MAT4);
			break;
		case graphics::EUniformValueType::VALUE_TYPE_MAT3:
			SetValueType(math::EValueType::VALUE_TYPE_MAT3);
			break;
		case graphics::EUniformValueType::VALUE_TYPE_MAT2:
			SetValueType(math::EValueType::VALUE_TYPE_MAT2);
			break;
		case graphics::EUniformValueType::VALUE_TYPE_VEC4:
			SetValueType(math::EValueType::VALUE_TYPE_VEC4);
			break;
		case graphics::EUniformValueType::VALUE_TYPE_VEC3:
			SetValueType(math::EValueType::VALUE_TYPE_VEC3);
			break;
		case graphics::EUniformValueType::VALUE_TYPE_VEC2:
			SetValueType(math::EValueType::VALUE_TYPE_VEC2);
			break;
		case graphics::EUniformValueType::VALUE_TYPE_FLOAT:
			SetValueType(math::EValueType::VALUE_TYPE_SCALAR);
			break;
		case graphics::EUniformValueType::VALUE_TYPE_INT:
			SetValueType(math::EValueType::VALUE_TYPE_SCALAR);
			break;
		/*case graphics::EUniformValueType::VALUE_TYPE_FLOAT_ARRAY:
			SetValueType(math::EValueType::VALUE_TYPE_VECTOR);
			break;
		case graphics::EUniformValueType::VALUE_TYPE_MAT4_ARRAY:
			SetValueType(math::EValueType::VALUE_TYPE_VECTOR);
			break;*/
		default:
			break;
		}
	}

	std::string CCustomTrack::GetTrackName()
	{
		return m_TargetValueName;
	}

	int CCustomTrack::GetParam_Int(const std::string Name)
	{
		return -1;
	}

	float CCustomTrack::GetParam_Float(const std::string Name)
	{
		return 0.0f;
	}

	std::string CCustomTrack::GetParam_String(const std::string Name)
	{
		if (Name == "ValueName")
		{
			return m_TargetValueName;
		}

		return std::string();
	}
}