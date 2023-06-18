#ifdef USE_VIEWER_CAMERA

#include "CViewerCamera.h"
#include "../Debug/Message/Console.h"

namespace camera
{
	CViewerCamera::CViewerCamera() :
		CCamera()
	{

	}

#ifdef USE_INPUT_SYSTEM
	void CViewerCamera::Update(float SecondsTime, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (InputState->IsDownMouseLeft())
		{
			float Speed = 0.1f;

			glm::vec4 Pos = glm::vec4(m_Pos, 1.0f) 
				* glm::mat4_cast(glm::angleAxis(InputState->GetDragAmount().x * Speed, glm::vec3(0.0f, 1.0f, 0.0f)))
				* glm::mat4_cast(glm::angleAxis(InputState->GetDragAmount().y * Speed, glm::vec3(1.0f, 0.0f, 0.0f)));

			m_Pos.x = Pos.x;
			m_Pos.y = Pos.y;
			m_Pos.z = Pos.z;
		}
	}
#endif // USE_INPUT_SYSTEM
}
#endif // USE_VIEWER_CAMERA