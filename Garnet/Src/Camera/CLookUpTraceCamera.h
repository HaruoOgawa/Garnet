#pragma once

#include <memory>
#include <vector>
#include "CCamera.h"

namespace object { class CNode; }

namespace camera
{
	// PosをOperateNode・CenterをLookUpNodeとしたカメラ
	// ViewDir方向中心の回転のみさらに指定することができる
	class CLookUpTraceCamera : public CCamera
	{
		std::shared_ptr<object::CNode> m_OperateNode;
		std::shared_ptr<object::CNode> m_LookUpNode;

		float m_ZAngle;

	public:
		CLookUpTraceCamera();
		virtual ~CLookUpTraceCamera();

		void SetOperateNode(const std::shared_ptr<object::CNode>& Node);
		void SetLookUpNode(const std::shared_ptr<object::CNode>& Node);

		void SetZAngle(float Angle);

		virtual void Update(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState) override;
	};
}