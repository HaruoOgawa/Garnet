#include "CNodeTrack.h"
#include "../Object/CNode.h"

namespace timeline
{
	CNodeTrack::CNodeTrack(const std::string& TrackID, int SamplerIndex, ETimelineSamplerTarget SamplerTarget, ENodeTrackTarget TrackTarget):
		CTimelineTrack(TrackID, SamplerIndex, SamplerTarget),
		m_TrackTarget(TrackTarget),
		m_Node(nullptr)
	{
		switch (m_TrackTarget)
		{
		case ENodeTrackTarget::NodeTrackTarget_None:
			break;

		case ENodeTrackTarget::NodeTrackTarget_EnabledFlag:
			SetValueType(math::EValueType::VALUE_TYPE_SCALAR);
			break;

		case ENodeTrackTarget::NodeTrackTarget_Translation:
			SetValueType(math::EValueType::VALUE_TYPE_VEC3);
			break;

		case ENodeTrackTarget::NodeTrackTarget_Rotation:
			SetValueType(math::EValueType::VALUE_TYPE_VEC4);
			break;

		case ENodeTrackTarget::NodeTrackTarget_Scale:
			SetValueType(math::EValueType::VALUE_TYPE_VEC3);
			break;

		default:
			break;
		}
	}

	CNodeTrack::~CNodeTrack()
	{
	}

	bool CNodeTrack::Update(float CurrentTime, const std::vector<float>& Value)
	{
		if (!m_Node) return true;

		switch (m_TrackTarget)
		{
		case ENodeTrackTarget::NodeTrackTarget_None:
			break;

		case ENodeTrackTarget::NodeTrackTarget_EnabledFlag:
			if (!UpdateEnabledFlag(Value)) return false;
			break;

		case ENodeTrackTarget::NodeTrackTarget_Translation:
			if (!UpdateTranslation(Value)) return false;
			break;

		case ENodeTrackTarget::NodeTrackTarget_Rotation:
			if (!UpdateRotation(Value)) return false;
			break;
		
		case ENodeTrackTarget::NodeTrackTarget_Scale:
			if (!UpdateScale(Value)) return false;
			break;

		default:
			break;
		}

		return true;
	}

	void CNodeTrack::AssignTrackContent(const std::shared_ptr<CTimelineTrackContent>& TrackContent)
	{
		m_Node = std::dynamic_pointer_cast<object::CNode>(TrackContent);
	}

	std::string CNodeTrack::GetTrackName()
	{
		std::string Name = std::string();

		switch (m_TrackTarget)
		{
		case ENodeTrackTarget::NodeTrackTarget_None:
			break;

		case ENodeTrackTarget::NodeTrackTarget_EnabledFlag:
			Name = "EnabledFlag";
			break;

		case ENodeTrackTarget::NodeTrackTarget_Translation:
			Name = "Translation";
			break;

		case ENodeTrackTarget::NodeTrackTarget_Rotation:
			Name = "Rotation";
			break;

		case ENodeTrackTarget::NodeTrackTarget_Scale:
			Name = "Scale";
			break;

		default:
			break;
		}

		return Name;
	}

	std::string CNodeTrack::CastNodeTrackTarget_Str(ENodeTrackTarget TrackTarget)
	{
		std::string dst = std::string();

		switch (TrackTarget)
		{
		case timeline::ENodeTrackTarget::NodeTrackTarget_None:
			dst = "None";
			break;
		case timeline::ENodeTrackTarget::NodeTrackTarget_EnabledFlag:
			dst = "EnabledFlag";
			break;
		case timeline::ENodeTrackTarget::NodeTrackTarget_Translation:
			dst = "Translation";
			break;
		case timeline::ENodeTrackTarget::NodeTrackTarget_Rotation:
			dst = "Rotation";
			break;
		case timeline::ENodeTrackTarget::NodeTrackTarget_Scale:
			dst = "Scale";
			break;
		case timeline::ENodeTrackTarget::NodeTrackTarget_Max:
			dst = "Max";
			break;
		default:
			break;
		}

		return dst;
	}

	bool CNodeTrack::UpdateEnabledFlag(const std::vector<float>& Value)
	{
		if (Value.size() != 1) return false;

		m_Node->SetEnabled((Value[0] != 0.0f));

		return true;
	}
	bool CNodeTrack::UpdateTranslation(const std::vector<float>& Value)
	{
		if (Value.size() != 3) return false;

		glm::vec3 Translation = glm::vec3(Value[0], Value[1], Value[2]);

		m_Node->SetPos(Translation);

		return true;
	}

	bool CNodeTrack::UpdateRotation(const std::vector<float>& Value)
	{
		if (Value.size() != 4) return false;

		glm::quat quat = glm::quat(Value[3], Value[0], Value[1], Value[2]);

		m_Node->SetRot(quat);

		return true;
	}

	bool CNodeTrack::UpdateScale(const std::vector<float>& Value)
	{
		if (Value.size() != 3) return false;

		glm::vec3 Scale = glm::vec3(Value[0], Value[1], Value[2]);

		m_Node->SetScale(Scale);

		return true;
	}
}