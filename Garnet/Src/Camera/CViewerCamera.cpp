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
		// マウス操作
		MouseAction(DeltaTime, InputState);
		
		// キーボード操作
		KeyAction(DeltaTime, InputState);
	}

	void CViewerCamera::MouseAction(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState)
	{
		// マウス操作
		if (InputState->IsDownMouseLeft()) // カメラの回転
		{
			float RotperSecond = 60.0; // 1秒あたり何度回転させるか

			const auto& ViewDir = GetViewDir();
			glm::vec3 AxisSide = glm::cross(ViewDir, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 AxisUp = glm::cross(ViewDir, AxisSide);

			glm::vec4 Pos = glm::vec4(m_Pos, 1.0f);

			// 原点に戻す
			Pos.x -= m_Center.x; Pos.y -= m_Center.y; Pos.z -= m_Center.z;

			// 回転
			Pos = Pos * glm::mat4_cast(glm::angleAxis(InputState->GetDragAmount().x * RotperSecond * DeltaTime * (-1.0f), AxisUp))
				* glm::mat4_cast(glm::angleAxis(InputState->GetDragAmount().y * RotperSecond * DeltaTime, AxisSide));

			// 元の平行移動成分を反映
			Pos.x += m_Center.x; Pos.y += m_Center.y; Pos.z += m_Center.z;

			//
			m_Pos.x = Pos.x;
			m_Pos.y = Pos.y;
			m_Pos.z = Pos.z;
		}
		else if (InputState->IsDownMouseRight()) // カメラの平行移動
		{
			float MoveRate = 60.0f; // 1秒あたりどれぐらい移動するか

			const auto& ViewDir = GetViewDir();
			glm::vec3 AxisSide = glm::cross(ViewDir, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 AxisUp = glm::cross(ViewDir, AxisSide);

			glm::vec3 Offset = AxisSide * InputState->GetDragAmount().x * MoveRate * DeltaTime * (-1.0f) + AxisUp * InputState->GetDragAmount().y * MoveRate * DeltaTime * (-1.0f);

			m_Center += Offset;
			m_Pos += Offset;
		}
		else if (InputState->IsMouseWheeled())
		{
			float MoveRate = 60.0f; // 1秒あたりどれぐらい移動するか

			const auto& ViewDir = GetViewDir();

			glm::vec3 Offset = ViewDir * MoveRate * DeltaTime * InputState->GetWheelScrollAmount().y;

			m_Center += Offset;
			m_Pos += Offset;
		}
	}

	void CViewerCamera::KeyAction(float DeltaTime, const std::shared_ptr<input::CInputState>& InputState)
	{
		// キーボード操作
		if (InputState->IsKeyDown())
		{
			//
			input::EKeyType KeyType = InputState->GetKeyType();

			//
			float MoveRate = 3.0f; // 1秒あたりどれぐらい移動するか

			const auto& ViewDir = GetViewDir();
			glm::vec3 AxisSide = glm::cross(ViewDir, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 AxisUp = glm::cross(ViewDir, AxisSide);

			glm::vec3 Offset = glm::vec3(0.0f);

			switch (KeyType)
			{
			case input::EKeyType::KEY_TYPE_NONE:
				break;
			case input::EKeyType::KEY_TYPE_W:
				Offset = ViewDir * MoveRate * DeltaTime * (1.0f);
				break;
			case input::EKeyType::KEY_TYPE_A:
				Offset = AxisSide * MoveRate * DeltaTime * (-1.0f);
				break;
			case input::EKeyType::KEY_TYPE_S:
				Offset = ViewDir * MoveRate * DeltaTime * (-1.0f);
				break;
			case input::EKeyType::KEY_TYPE_D:
				Offset = AxisSide * MoveRate * DeltaTime * (1.0f);
				break;
			default:
				break;
			}

			m_Center += Offset;
			m_Pos += Offset;
		}
	}
#endif // USE_INPUT_SYSTEM
}
#endif // USE_VIEWER_CAMERA