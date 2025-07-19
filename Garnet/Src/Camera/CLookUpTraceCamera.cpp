#include "CLookUpTraceCamera.h"
#include "../Object/CNode.h"

namespace camera
{
	CLookUpTraceCamera::CLookUpTraceCamera():
		CCamera(),
		m_OperateNode(nullptr),
		m_LookUpNode(nullptr),
		m_ZAngle(0.0f)
	{
	}

	CLookUpTraceCamera::~CLookUpTraceCamera()
	{
	}

	void CLookUpTraceCamera::SetOperateNode(const std::shared_ptr<object::CNode>& Node)
	{
		m_OperateNode = Node;
	}

	void CLookUpTraceCamera::SetLookUpNode(const std::shared_ptr<object::CNode>& Node)
	{
		m_LookUpNode = Node;
	}

	void CLookUpTraceCamera::SetZAngle(float Angle)
	{
		m_ZAngle = Angle;
	}

	void CLookUpTraceCamera::Update(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (m_OperateNode) m_Pos = m_OperateNode->GetWorldPos();

		if (m_LookUpNode) m_Center = m_LookUpNode->GetWorldPos();

		glm::vec3 ViewDir = glm::normalize(m_Pos - m_Center);
		glm::quat Rot = glm::angleAxis(m_ZAngle, ViewDir);

		m_UpVector = Rot * glm::vec3(0.0f, 1.0f, 0.0f);
	}
}