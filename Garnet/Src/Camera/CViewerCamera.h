#pragma once

#ifdef USE_VIEWER_CAMERA
#include "CCamera.h"

namespace camera
{
	class CViewerCamera : public CCamera
	{
	public:
		CViewerCamera();
		virtual ~CViewerCamera() = default;

#ifdef USE_INPUT_SYSTEM
		virtual void Update(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState) override;
#endif // USE_INPUT_SYSTEM
	};
}
#endif // USE_VIEWER_CAMERA