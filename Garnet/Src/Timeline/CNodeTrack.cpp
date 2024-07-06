#include "CNodeTrack.h"
#include "../Object/CNode.h"

namespace timeline
{
	CNodeTrack::CNodeTrack(const std::string& TrackID, int SamplerIndex, animation::EAnimationTarget AnimationTarget, ENodeTrackTarget TrackTarget):
		CTimelineTrack(TrackID, SamplerIndex, AnimationTarget),
		m_TrackTarget(TrackTarget),
		m_Node(nullptr)
	{
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