#include "CTraceCamera.h"
#include "../Object/CNode.h"

namespace camera
{
	CTraceCamera::CTraceCamera():
		CCamera(),
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
		m_UpVector = YVector;
	}

	/*glm::mat4 CTraceCamera::GetViewMatrix()
	{
		if (!m_TargetNode) return glm::mat4(1.0f);

		const auto& NodePos = m_TargetNode->GetPos();
		glm::quat NodeRotate = m_TargetNode->GetRot();

		m_Pos = NodePos;

		// カメラの回転行列
		glm::mat4 rotation = glm::mat4_cast(glm::conjugate(NodeRotate));

		// カメラの翻訳行列
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), -m_Pos);

		// 回転行列と翻訳行列を掛け合わせてビュー行列を作成
		glm::mat4 vmat = rotation * translation;

		return vmat;
	}*/
}