#include "CTraceCamera.h"
#include "../Object/CNode.h"

namespace camera
{
	CTraceCamera::CTraceCamera():
		m_TargetNode(nullptr)
	{
	}

	void CTraceCamera::SetTargetNode(const std::shared_ptr<object::CNode>& TargetNode)
	{
		m_TargetNode = TargetNode;
	}

	void CTraceCamera::Update(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_TargetNode) return;

		const auto& NodePos = m_TargetNode->GetPos();
		const auto& NodeRotate = m_TargetNode->GetRot();

		m_Pos = NodePos;

		//glm::vec3 XVector = glm::normalize(glm::vec3(glm::toMat4(NodeRotate) * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
		glm::vec3 YVector = glm::normalize(glm::vec3(glm::toMat4(NodeRotate) * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
		glm::vec3 ZVector = glm::normalize(glm::vec3(glm::toMat4(NodeRotate) * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));

		m_Center = ZVector + NodePos;
		m_Up = YVector;
	}
}