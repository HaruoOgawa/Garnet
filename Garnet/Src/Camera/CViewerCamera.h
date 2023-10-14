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
#ifdef USE_INPUT_SYSTEM
		void MouseAction(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState);
		void KeyAction(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState);
#endif // USE_INPUT_SYSTEM
	public:
		CViewerCamera();
		virtual ~CViewerCamera() = default;

		virtual void SetPos(const glm::vec3& Pos) override;
		virtual void SetCenter(const glm::vec3& Center) override;

#ifdef USE_INPUT_SYSTEM
		virtual void Update(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState) override;
#endif // USE_INPUT_SYSTEM
	};
}
#endif // USE_VIEWER_CAMERA