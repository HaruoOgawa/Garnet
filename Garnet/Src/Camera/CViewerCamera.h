#pragma once

#ifdef USE_VIEWER_CAMERA
#include "CCamera.h"

namespace camera
{
	class CViewerCamera : public CCamera
	{
		const float m_MinDistance;
		const float m_MaxDistance;
	private:
		void MouseAction(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState);
		void KeyAction(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState);
	public:
		CViewerCamera();
		CViewerCamera(const glm::vec3& Pos, const glm::vec3& Center, const glm::vec3& UpVector);
		virtual ~CViewerCamera() = default;

		virtual void SetPos(const glm::vec3& Pos) override;
		virtual void SetCenter(const glm::vec3& Center) override;

		virtual void Update(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState) override;
	};
}
#endif // USE_VIEWER_CAMERA