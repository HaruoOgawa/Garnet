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
		if (InputState->IsDownMouseLeft()) // ƒJƒƒ‰‚Ì‰ñ“]
		{
			float RotperSecond = 60.0; // 1•b‚ ‚½‚è‰½“x‰ñ“]‚³‚¹‚é‚©

			const auto& ViewDir = GetViewDir();
			glm::vec3 AxisSide = glm::cross(ViewDir, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 AxisUp = glm::cross(ViewDir, AxisSide);

			glm::vec4 Pos = glm::vec4(m_Pos, 1.0f) 
				* glm::mat4_cast(glm::angleAxis(InputState->GetDragAmount().x * RotperSecond * DeltaTime * (-1.0f), AxisUp))
				* glm::mat4_cast(glm::angleAxis(InputState->GetDragAmount().y * RotperSecond * DeltaTime, AxisSide));

			m_Pos.x = Pos.x;
			m_Pos.y = Pos.y;
			m_Pos.z = Pos.z;
		}
		else if (InputState->IsDownMouseRight()) // ƒJƒƒ‰‚Ì•½sˆÚ“®
		{
			float MoveRate = 60.0f; // 1•b‚ ‚½‚è‚Ç‚ê‚®‚ç‚¢ˆÚ“®‚·‚é‚©

			const auto& ViewDir = GetViewDir();
			glm::vec3 AxisSide = glm::cross(ViewDir, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 AxisUp = glm::cross(ViewDir, AxisSide);

			glm::vec3 Offset = AxisSide * InputState->GetDragAmount().x * MoveRate * DeltaTime * (-1.0f) + AxisUp * InputState->GetDragAmount().y * MoveRate * DeltaTime * (-1.0f);
			
			m_Center += Offset;
			m_Pos += Offset;
		}
	}
#endif // USE_INPUT_SYSTEM
}
#endif // USE_VIEWER_CAMERA