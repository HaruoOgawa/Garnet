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
	void CViewerCamera::Update(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (InputState->IsDownMouseLeft())
		{
			float Speed = InputState->GetMouseRotSpeed();

			float RotperSecond = 60.0; // ˆê•b‚ ‚½‚è‰½“x‰ñ“]‚³‚¹‚é‚©

			const auto& ViewDir = GetViewDir();
			glm::vec3 RotAxisX = glm::cross(ViewDir, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 RotAxisY = glm::cross(ViewDir, RotAxisX);

			glm::vec4 Pos = glm::vec4(m_Pos, 1.0f) 
				* glm::mat4_cast(glm::angleAxis(InputState->GetDragAmount().x * RotperSecond * DeltaTime * (-1.0f), RotAxisY))
				* glm::mat4_cast(glm::angleAxis(InputState->GetDragAmount().y * RotperSecond * DeltaTime, RotAxisX));

			m_Pos.x = Pos.x;
			m_Pos.y = Pos.y;
			m_Pos.z = Pos.z;
		}
	}
#endif // USE_INPUT_SYSTEM
}
#endif // USE_VIEWER_CAMERA