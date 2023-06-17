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

		virtual void Update(float SecondsTime) override;
	};
}
#endif // USE_VIEWER_CAMERA