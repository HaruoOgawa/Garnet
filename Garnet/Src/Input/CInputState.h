#pragma once

#ifdef USE_INPUT_SYSTEM
#include <glm/glm.hpp>

namespace input
{
	class CInputState
	{
		glm::vec2 m_MousePos;
		glm::vec2 m_PrevMousePos;

		glm::vec2 m_WheelScrollAmount;

		bool m_IsLocked;

		bool m_OnDownMouseLeft;
		bool m_OnDownMouseRight;
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
	};
}
#endif // #ifdef USE_INPUT_SYSTEM