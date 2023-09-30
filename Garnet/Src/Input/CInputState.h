#pragma once

#ifdef USE_INPUT_SYSTEM
#include <glm/glm.hpp>

namespace input
{
	enum class EKeyType
	{
		KEY_TYPE_NONE,

		KEY_TYPE_W,
		KEY_TYPE_A,
		KEY_TYPE_S,
		KEY_TYPE_D,
	};

	class CInputState
	{
		glm::vec2 m_MousePos;
		glm::vec2 m_PrevMousePos;

		glm::vec2 m_WheelScrollAmount;

		bool m_IsLocked;

		bool m_OnDownMouseLeft;
		bool m_OnDownMouseRight;

		bool m_OnKeyDown;
		EKeyType m_KeyType;
	public:
		CInputState(float MouseRotSpeed);
		virtual ~CInputState();

		void Clear();

		void StartMousePos(const glm::vec2& MousePos);
		void SetMousePos(const glm::vec2& MousePos);

		void SetWheelScrollAmount(const glm::vec2& ScrollAmount);
		const glm::vec2& GetWheelScrollAmount() const;
		bool IsMouseWheeled();

		void SetLock(bool State);
		bool IsLocked()const;
		
		void SetDownMouseLeft(bool OnDownMouseLeft);
		bool IsDownMouseLeft()const;
		
		void SetDownMouseRight(bool OnDownMouseRight);
		bool IsDownMouseRight()const;
		glm::vec2 GetDragAmount();

		void SetKeyDown(bool KeyDown);
		bool IsKeyDown()const;

		void SetKeyType(EKeyType KeyType);
		EKeyType GetKeyType()const;
	};
}
#endif // #ifdef USE_INPUT_SYSTEM