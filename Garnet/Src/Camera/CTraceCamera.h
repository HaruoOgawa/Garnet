#pragma once

#include <memory>
#include "CCamera.h"

namespace object { class CNode; }

namespace camera
{
	class CTraceCamera : public CCamera
	{
		std::shared_ptr<object::CNode> m_TargetNode;
	public:
		CTraceCamera();
		virtual ~CTraceCamera() = default;

		void SetTargetNode(const std::shared_ptr<object::CNode>& TargetNode);

		virtual void Update(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState) override;

		//virtual glm::mat4 GetViewMatrix() override;
	};
}